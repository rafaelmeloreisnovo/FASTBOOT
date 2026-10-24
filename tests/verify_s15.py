#!/usr/bin/env python3
"""Host-only evidence audit; NEVER linked into freestanding L0."""
import copy
import json
import re
import sys
from pathlib import Path

SHA = re.compile(r"^[0-9a-f]{40}$")
ALLOWED = {"NOT_RUN", "NOT_OBSERVED", "UNVERIFIED", "BLOCKED", "HYPOTHESIS", "OBSERVED_GAP", "PASS_SCOPED"}
GATES = {f"G14-{i:02d}" for i in range(1, 9)}


def require(test, message):
    if not test:
        raise ValueError(message)


def validate(prior, current):
    require(prior.get("schema") == "fastboot-omega/s14-evidence-gates/v1", "prior schema")
    require(current.get("schema") == "fastboot-omega/s15-falsifiability/v1", "current schema")
    require(current.get("claim_allowed") is False and current.get("promotion") is False, "promotion denied")
    require(current.get("unresolved_token_zero") is False, "TOKEN_VAZIO cannot be zero")
    require(SHA.fullmatch(current.get("parent_merge_sha", "")), "exact parent SHA")
    require(current.get("source_scope") == "RESEARCH_ONLY", "unscoped claim")
    prior_gates = prior.get("gates")
    hypotheses = current.get("hypotheses")
    require(isinstance(prior_gates, list) and isinstance(hypotheses, list), "gate structure")
    require({g.get("id") for g in prior_gates} == GATES and len(prior_gates) == 8, "prior gate set")
    require({h.get("gate") for h in hypotheses} == GATES and len(hypotheses) == 8, "hypothesis set")
    for g in prior_gates:
        require(g.get("priority") in ("P0", "P1", "P2"), "priority missing")
        require(g.get("status") in ALLOWED and g["status"] != "PASS_SCOPED", "prior approval not observed")
        require(bool(g.get("acceptance")) and bool(g.get("rollback")), "gate lacks acceptance or rollback")
    for h in hypotheses:
        require(all(isinstance(h.get(k), str) and len(h[k].strip()) >= 18 for k in
                    ("hypothesis", "falsifier", "experiment", "acceptance", "rollback")), "unfalsifiable hypothesis")
        require(h.get("status") in ALLOWED - {"PASS_SCOPED"}, "hypothesis promoted without execution")
        require(isinstance(h.get("references"), list), "provenance links missing")
    require(len(current.get("receipts", [])) >= 1, "no historical receipts")
    receipt_by_id = {}
    for r in current["receipts"]:
        require(r.get("id") not in receipt_by_id, "duplicate receipt")
        require(SHA.fullmatch(r.get("source_sha", "")), "receipt SHA missing")
        require(r.get("result") == "PASS_SCOPED", "receipt cannot claim global pass")
        require(r.get("environment") and r.get("url", "").startswith("https://github.com/"), "receipt provenance")
        receipt_by_id[r["id"]] = r
    for c in current.get("claims", []):
        r = receipt_by_id.get(c.get("receipt"))
        require(r is not None, "orphan claim")
        require(c.get("source_sha") == r["source_sha"], "claim inherited a different SHA")
        require(c.get("scope") == r.get("scope") and c.get("status") == "PASS_SCOPED", "claim exceeded receipt scope")
        require(c.get("source_sha") != current["parent_merge_sha"], "merge inherits PR HEAD PASS")
    require(current.get("device_execution") == "NOT_RUN", "physical evidence fabricated")
    require(current.get("usb_execution") == "NOT_RUN", "USB evidence fabricated")
    require(current.get("provider_enforcement") == "OBSERVED_GAP", "missing provider gate")
    return len(hypotheses)


def self_test(prior, current):
    assert validate(prior, current) == 8
    mutations = [
        lambda x: x.update(promotion=True),
        lambda x: x.update(claim_allowed=True),
        lambda x: x.update(unresolved_token_zero=True),
        lambda x: x.update(parent_merge_sha="not-a-sha"),
        lambda x: x.update(source_scope="PRODUCTION"),
        lambda x: x["hypotheses"].pop(),
        lambda x: x["hypotheses"][0].update(gate="G14-99"),
        lambda x: x["hypotheses"][0].update(falsifier=""),
        lambda x: x["hypotheses"][0].update(status="PASS_SCOPED"),
        lambda x: x["receipts"][0].update(source_sha="0"),
        lambda x: x["claims"][0].update(receipt="missing"),
        lambda x: x["claims"][0].update(source_sha=x["parent_merge_sha"]),
        lambda x: x["claims"][0].update(scope="PRODUCTION"),
        lambda x: x.update(device_execution="PASS_SCOPED"),
        lambda x: x.update(usb_execution="PASS_SCOPED"),
        lambda x: x.update(provider_enforcement="PASS_SCOPED"),
    ]
    for i, mutate in enumerate(mutations, 1):
        candidate = copy.deepcopy(current)
        mutate(candidate)
        try:
            validate(prior, candidate)
        except ValueError:
            continue
        raise AssertionError(f"mutation {i} falsely accepted")
    return len(mutations)


def main():
    if len(sys.argv) != 3:
        print("Usage: verify_s15.py S14.json S15.json", file=sys.stderr)
        return 64
    prior = json.loads(Path(sys.argv[1]).read_text(encoding="utf-8"))
    current = json.loads(Path(sys.argv[2]).read_text(encoding="utf-8"))
    try:
        count = validate(prior, current)
        negative_count = self_test(prior, current)
    except (ValueError, AssertionError) as ex:
        print(f"FAIL_CLOSED: {ex}", file=sys.stderr)
        return 1
    print(f"PASS_SCOPED: {count} falsifiable gates; {negative_count} semantic negatives rejected; no hardware or promotion claim")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
