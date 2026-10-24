# FASTBOOT Ω S13 — invariants and typed gaps (NO PROMOTION)

BASE SHA: `40f7440d241947c7cea5be5d1f6a8f657e90dea6`.
This branch is a separate documentation-only successor, not a production release.
The L0 CPU core remains unchanged; S13 L1 privacy/path-guard implementation is
locally tested but NOT COMMITTED as part of this documentation.

## Provider-observed evidence (2026-10-24)
- main protected=false and public repository; reported rulesets=[].
- Detailed branch protection GET was denied HTTP 403 to the integration.
- A tracking issue exists: https://github.com/rafaelmeloreisnovo/FASTBOOT/issues/3.
- Prior x86, hosted native AArch64, and emulated ARMv7 CI passed at an earlier exact SHA;
  those PASS results do NOT automatically apply to S13.

## Nonzero empty-token type system
OBSERVED / BLOCKED / NOT_RUN / UNVERIFIED / HYPOTHESIS / MITIGATED_LOCAL are
distinct epistemic states. Type families:
1. ENFORCEMENT_ABSENT, PROVIDER_AUTHORITY, PROVIDER_CAPABILITY
2. EXECUTION_NOT_OBSERVED, PHYSICAL_DEVICE_NOT_RUN, SOC_SPECIFIC_UNKNOWN
3. REPRODUCIBILITY_GAP, BUILD_TOOL_EOL_RISK, LICENSE_REVIEW_REQUIRED
4. IDENTITY_UNVERIFIED, RACE_UNRESOLVED, COMPATIBILITY_UNVERIFIED
5. PRIVACY_UNVERIFIED, LOCAL_MITIGATION_NOT_PROMOTED
6. INDEPENDENT_REVIEW_REQUIRED, ACCESSIBILITY_NOT_VALIDATED
7. CHILD_PROTECTION_NOT_VALIDATED, LOCAL_CULTURE_REVIEW_REQUIRED
8. PERFORMANCE_NOT_MEASURED, UNKNOWN_UNKNOWN_DISCOVERY

Every unresolved token needs evidence status, falsifiable acceptance criterion,
responsible role, mitigation and rollback. A hypothetical discovery cannot be
reported as an observed failure. Zero missing tokens does not mean zero risk.

## S13 local evidence, not yet remotely reproduced
- L1 Linux host candidate limits USB path shape to /dev/bus/usb/DDD/DDD,
  rejects malformed paths BEFORE open, requests O_NOFOLLOW, and centralizes TX/RX
  buffer cleanup after early exits. It does not authenticate hardware.
- USB-host mocked privacy and earlier S10 regression tests PASS locally.
- Structured local ledger covers 21 tokens, 8 discovery candidates, 10 gates;
  its validator rejects 11 unsafe semantic mutations.
- No execution on user phone or real USB/OTG has been observed; no S13 CI result.

## Hard promotion gates
- Publish exact S13 source and run the same tests at its SHA.
- Enforce mandatory independent review and required CI checks on main; verify that
  the provider blocks an unapproved merge, not merely that policy prose exists.
- Obtain consented physical ARM32/ARM64 and read-only USB/OTG evidence.
- Perform authorship, privacy, child-protection, cultural-accessibility review.
- Never delete predecessor work or merge unverified candidates automatically.

SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM.
TOKEN_VAZIO != 0. claim_allowed=false. promotion=false.
