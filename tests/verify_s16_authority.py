#!/usr/bin/env python3
"""S16: fail-closed provider/physical release-eligibility observer, host-only.

No direct branch-protection mutation and no release/merge. A CI workflow
belonging to a PR is itself untrusted until externally enforced.
"""
import argparse
import json
import os
import re
import sys
import urllib.error
import urllib.request
from pathlib import Path

SHA = re.compile(r"^[0-9a-f]{40}$")


def source_observations(repo, head, pr_number, token):
    base = "https://api.github.com/repos/" + repo
    hdr = {"Accept": "application/vnd.github+json", "X-GitHub-Api-Version": "2022-11-28"}
    if token:
        hdr["Authorization"] = "Bearer " + token

    def read(path):
        try:
            q = urllib.request.Request(base + path, headers=hdr)
            with urllib.request.urlopen(q, timeout=20) as reply:
                return json.load(reply), "OBSERVED"
        except urllib.error.HTTPError as exc:
            return None, "PROVIDER_AUTHORITY_HTTP_" + str(exc.code)
        except (urllib.error.URLError, TimeoutError, ValueError) as exc:
            return None, "PROVIDER_UNOBSERVED_" + type(exc).__name__

    branch, bst = read("/branches/main")
    protection, pst = read("/branches/main/protection")
    rulesets, rst = read("/rulesets")
    pr, prst = read("/pulls/" + str(pr_number))
    reviews, rvst = read("/pulls/" + str(pr_number) + "/reviews?per_page=100")
    return {
        "schema": "fastboot-omega/s16-provider-observation/v1",
        "source_sha": head, "repository": repo, "pr_number": pr_number,
        "branch": branch, "branch_read_status": bst,
        "protection": protection, "protection_read_status": pst,
        "rulesets": rulesets, "rulesets_read_status": rst,
        "pr": pr, "pr_read_status": prst,
        "reviews": reviews, "reviews_read_status": rvst,
        "physical_receipt": None,
        "artifact_rebuild_verified": False,
        "claim_allowed": False, "promotion": False,
    }


def assess(o):
    results = []

    def append(name, good, missing_type, note):
        results.append({"gate": name, "status": "PASS_SCOPED" if good else "TOKEN_VAZIO",
                        "token_type": None if good else missing_type,
                        "evidence": note, "mitigation": "No release/merge; preserve exact SHA and all predecessor receipts",
                        "rollback": "Preserve branch and main history; no force push or automatic delete"})

    head = o.get("source_sha", "")
    valid_sha = isinstance(head, str) and SHA.fullmatch(head) is not None
    append("EXACT_HEAD", valid_sha, "SOURCE_SHA_INVALID", "SHA syntax only; not a repository verification")
    if not valid_sha:
        return results

    branch = o.get("branch") or {}
    append("MAIN_PROTECTED", branch.get("protected") is True,
           "ENFORCEMENT_ABSENT" if o.get("branch_read_status") == "OBSERVED" else "PROVIDER_AUTHORITY",
           "GET /branches/main: " + str(o.get("branch_read_status")))

    pr = o.get("pr") or {}
    latest_head = (pr.get("head") or {}).get("sha")
    append("PR_SHA", latest_head == head and pr.get("merged") is False and pr.get("draft") is True,
           "SOURCE_DRIFT_OR_PROMOTION", "PR head and draft status must equal expected source")

    rulesets = o.get("rulesets") or []
    active = isinstance(rulesets, list) and any(
        isinstance(x, dict) and x.get("enforcement") == "active" and
        x.get("target") == "branch" for x in rulesets
    )
    append("ACTIVE_BRANCH_RULESET", active and o.get("rulesets_read_status") == "OBSERVED",
           "RULESET_ENFORCEMENT_ABSENT" if o.get("rulesets_read_status") == "OBSERVED" else "PROVIDER_AUTHORITY",
           "Require observed, active branch ruleset; branch-specific coverage requires independent verification")

    protection = o.get("protection") or {}
    reviews_policy = protection.get("required_pull_request_reviews") or {}
    checks_policy = protection.get("required_status_checks") or {}
    has_required_approval = int(reviews_policy.get("required_approving_review_count") or 0) >= 1
    has_required_checks = bool(checks_policy.get("contexts") or checks_policy.get("checks"))
    append("ENFORCED_REVIEW_POLICY", has_required_approval and o.get("protection_read_status") == "OBSERVED",
           "PROVIDER_AUTHORITY" if o.get("protection_read_status") != "OBSERVED" else "REVIEW_POLICY_ABSENT",
           "Branch protection required approving review count >= 1")
    append("ENFORCED_CHECK_POLICY", has_required_checks and o.get("protection_read_status") == "OBSERVED",
           "PROVIDER_AUTHORITY" if o.get("protection_read_status") != "OBSERVED" else "STATUS_POLICY_ABSENT",
           "Branch protection must specify required exact-SHA status checks")

    reviews = o.get("reviews") or []
    author = ((pr.get("user") or {}).get("login") or "").lower()
    approvals = [r for r in reviews if r.get("state") == "APPROVED" and
                 r.get("commit_id") == head and
                 ((r.get("user") or {}).get("login") or "").lower() != author]
    append("INDEPENDENT_APPROVAL", bool(approvals) and o.get("reviews_read_status") == "OBSERVED", "INDEPENDENT_REVIEW_NOT_OBSERVED",
           "Independent and exact-head reviewer submission is required")

    append("INDEPENDENT_REBUILD", o.get("artifact_rebuild_verified") is True,
           "INDEPENDENT_REBUILD_NOT_RUN", "Two separately provisioned runner manifests must match")
    physical = o.get("physical_receipt") or {}
    append("PHYSICAL_DEVICE", physical.get("source_sha") == head and
           physical.get("explicit_consent") is True and
           physical.get("read_only_usb") is True and
           physical.get("independent_review") is True,
           "PHYSICAL_USB_NOT_RUN", "Physical evidence is NOT provided by a QEMU or hosted ARM64 runner")
    append("NO_PROMOTION", o.get("claim_allowed") is False and o.get("promotion") is False,
           "UNAUTHORIZED_PROMOTION", "No automatic release or merge is authorized")
    return results


def main(argv=None):
    p = argparse.ArgumentParser()
    p.add_argument("--fixture")
    p.add_argument("--repo", default="rafaelmeloreisnovo/FASTBOOT")
    p.add_argument("--head")
    p.add_argument("--pr", type=int)
    p.add_argument("--output", default="build/s16-provider-receipt.json")
    p.add_argument("--enforce", action="store_true")
    p.add_argument("--rebuild-pass", action="store_true")
    args = p.parse_args(argv)
    if args.fixture:
        obs = json.loads(Path(args.fixture).read_text())
    else:
        if not args.head or not args.pr or not SHA.fullmatch(args.head):
            p.error("--head exact 40-digit lowercase SHA and --pr required")
        obs = source_observations(args.repo, args.head, args.pr,
                                  os.environ.get("GH_TOKEN", ""))
        obs["artifact_rebuild_verified"] = bool(args.rebuild_pass)
    results = assess(obs)
    eligibility = bool(results) and all(r["status"] == "PASS_SCOPED" for r in results)
    receipt = {"schema": "fastboot-omega/s16-provider-receipt/v1",
               "source_sha": obs.get("source_sha"), "eligibility": "CANDIDATE_ONLY" if eligibility else "NO_GO",
               "claim_allowed": False, "promotion": False, "gates": results}
    path = Path(args.output)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(receipt, indent=2, sort_keys=True) + "\n")
    for r in results:
        print(r["gate"], r["status"], r["token_type"] or "SCOPED")
    print("ELIGIBILITY=", receipt["eligibility"], "RECEIPT=", str(path))
    # An eligible candidate is still not a release: human act/permissions remain external.
    if args.enforce and not eligibility:
        return 3
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
