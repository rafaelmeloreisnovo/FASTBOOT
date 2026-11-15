# FASTBOOT AGENTS — canonical operating contract

Version: 2.0
Authority: this file is the repository-wide navigation and change contract. `AGENT.md` is a compatibility pointer only.

## Invariants

- Preserve identity, origin, predecessor, license, evidence, unknowns and rollback.
- `SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM`.
- `TOKEN_VAZIO` is typed absence; it is not zero, false, failure or PASS.
- `IMPLEMENTED_UNTESTED != PASS`; scoped green checks never prove physical USB operation.
- Never erase historical evidence. Correct with a reviewable successor.
- Never weaken the S16 authority/physical-evidence NO-GO to obtain green CI.

## Unique CI composition

`.github/workflows/fastboot-orchestrator.yml` is the only automatic PR/push entrypoint.
The four specialist workflows are reusable units invoked exactly once by the orchestrator and may also be dispatched manually:

1. evidence — schema, falsifiers and receipts;
2. ISA — freestanding host, native ARM64 and emulated ARMv7 checks;
3. Mi Assistant — fail-closed Android/Java source contract;
4. S16 closure — independent manifests followed by enforced authority/physical NO-GO.

All third-party JavaScript actions must be immutable SHA pins whose declared runtime is Node 24. The static composition test enforces the current allowlist.

## Required change receipt

Every behavior, workflow or contract modification must include concise documentation in the same PR:

- baseline SHA and modified paths;
- purpose and bounded delta;
- tests actually run and exact evidence scope;
- typed gaps/falsifiers;
- rollback by successor/revert;
- next smallest reversible action.

Use `receipts/` for execution receipts and update `docs/WORKFLOW_AUDIT.md` when workflow topology or pins change.

## Device boundary

Host is Motorola Moto E7 Power / Android 10 / reported ARMv7. Target is Redmi 12 (23053RN02A). Allowed protocol is bounded, read-only Fastboot `getvar`. Flash, erase, unlock, boot, reboot, partition writes, exploit paths, remote command channels and uploads of device identifiers are outside scope. Physical USB permission, descriptors, endpoints and target replies remain TOKEN_VAZIO until measured on the devices.

## Evidence routing

Intent → canonical source → smallest role set → execution → evidence → receipt → R3.
Use typed edges: `implements`, `uses`, `tests`, `observes`, `derived_from`, `supports`, `contradicts`, `supersedes`, `rollback_of`.
An unsupported edge is `HYPOTHESIS_EDGE`.

Close every material change with:
- F_ok: what the named checks support;
- F_gap: typed unknowns;
- F_next: one reversible discriminating action.
