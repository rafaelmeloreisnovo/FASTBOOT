#!/usr/bin/env python3
"""Synthetic negative controls of the S16 provider gate. No network."""
import copy
from verify_s16_authority import assess

SHA = "a" * 40
BASE = {
    "source_sha": SHA, "branch_read_status": "OBSERVED", "branch": {"protected": True},
    "protection_read_status": "OBSERVED",
    "protection": {
        "required_pull_request_reviews": {"required_approving_review_count": 1},
        "required_status_checks": {"contexts": ["source-bound-research-gates"]}},
    "pr": {"head": {"sha": SHA}, "merged": False, "draft": True,
           "user": {"login": "author"}},
    "reviews": [{"state": "APPROVED", "commit_id": SHA, "user": {"login": "independent"}}],
    "rulesets_read_status": "OBSERVED",
    "rulesets": [{"id": 1234, "target": "branch", "enforcement": "active"}],
    "reviews_read_status": "OBSERVED",
    "artifact_rebuild_verified": True,
    "physical_receipt": {"source_sha": SHA, "explicit_consent": True,
                         "read_only_usb": True, "independent_review": True},
    "claim_allowed": False, "promotion": False,
}

def eligible(obs):
    return all(v["status"] == "PASS_SCOPED" for v in assess(obs))

assert eligible(BASE), "control fixture should be logically eligible only"
mutants = [
    lambda x: x.update(source_sha="not-sha"),
    lambda x: x["branch"].update(protected=False),
    lambda x: x.update(branch=None, branch_read_status="PROVIDER_AUTHORITY_HTTP_403"),
    lambda x: x["pr"]["head"].update(sha="b"*40),
    lambda x: x["pr"].update(merged=True),
    lambda x: x["pr"].update(draft=False),
    lambda x: x["protection"].update(required_pull_request_reviews=None),
    lambda x: x.update(protection_read_status="PROVIDER_AUTHORITY_HTTP_403", protection=None),
    lambda x: x["protection"].update(required_status_checks={}),
    lambda x: x.update(reviews=[]),
    lambda x: x["reviews"][0].update(commit_id="b"*40),
    lambda x: x["reviews"][0]["user"].update(login="author"),
    lambda x: x.update(rulesets=[]),
    lambda x: x.update(rulesets_read_status="PROVIDER_AUTHORITY_HTTP_403"),
    lambda x: x.update(reviews_read_status="PROVIDER_UNOBSERVED_TimeoutError"),
    lambda x: x.update(artifact_rebuild_verified=False),
    lambda x: x.update(physical_receipt=None),
    lambda x: x["physical_receipt"].update(read_only_usb=False),
    lambda x: x["physical_receipt"].update(explicit_consent=False),
    lambda x: x["physical_receipt"].update(independent_review=False),
    lambda x: x.update(claim_allowed=True),
    lambda x: x.update(promotion=True),
]
for index, change in enumerate(mutants, 1):
    candidate = copy.deepcopy(BASE)
    change(candidate)
    assert not eligible(candidate), f"FAILED to reject mutated evidence {index}"
print(f"PASS_SCOPED={len(mutants)} semantic authority negatives rejected; no provider or physical claim")
