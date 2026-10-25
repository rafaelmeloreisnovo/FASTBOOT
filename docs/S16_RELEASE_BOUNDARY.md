# FASTBOOT Ω S16 — Fechamento por comparação independente e autoridade real

Classification: safety/governance candidate, not firmware, hardware attestation,
research publication, certification or production release.

## First, falsify the unsafe claim

- `SOURCE` **≠** `ARTIFACT` **≠** `EXECUTION` **≠** `EVIDENCE` **≠** `CLAIM`.
- A source CI PASS and an exact merge SHA are not automatically the same receipt.
- Branch `main` reported `protected=false` and no rulesets in the observed API.
- The GitHub connector used in this project exposes branch reads and file/commit
  writes, **but not an authorized branch-protection or ruleset setting operation**.
  No documentation-only workflow can enforce required checks against a repository
  administrator. GitHub issue #3 tracks provider-side authority and acceptance.
- Previous PR #6 was merged with no review submissions returned by the query.
  The merge actor, consent and any separate human review are **not established**.

## Implementation

1. **Two fresh Ubuntu CI workers** each build the nine self-contained ASM ELF
   modules for ARMv7, AArch64 and x86_64 at the same SHA using existing L0
   freestanding gates. The build uses clang/LLVM, not runtime libc in the ASM.
2. Each hashes 9 individual `.text` binaries, 3 concatenated architecture
   bundles, and 3 offset layouts = **15 artifact manifests**. A comparison job
   requires byte-identical outputs. Two CI machines are not independently
   operated trust roots, nor a compiler attestation or proof of correctness.
3. The **release authority gate** queries GitHub `main` protection, active
   ruleset status, PR draft+SHA, review policies, required checks and a review
   submission by an independent person for that exact SHA. If API rights are
   missing, it returns a typed `PROVIDER_AUTHORITY` uncertainty, not false PASS.
4. A machine-readable receipt retains typed `TOKEN_VAZIO`, precise reason,
   mitigation and rollback for each NO_GO. The gate refuses promotion when no
   physical consenting USB read-only receipt exists, even after source checks.
5. `selftest_s16_authority.py` rejects **22 synthetic mutations**; this proves
   denial logic over those cases only. No production certification is claimed.

## Crucial authority boundary

The GitHub CI `release-no-go` job intentionally **fails** when protections,
required approvals or a physical receipt are absent. A red check is correct
and safer than a green but unjustified claim. Because `main` remains unprotected,
**GitHub can still allow a merge despite this failed job**. Administrative
branch protection/rulesets and a provider-observed blocked unreviewed merge are
still required to close P0. Do NOT attempt destructive reversions, delete a
branch or enable auto-merge. This document cannot impersonate the administrator.

## Acceptance / rollback

**Success of software-only S16**: two manifests identical, exact source SHA
in all runners, 22 negative mutations rejected, NO_GO receipt generated with
real provider state. **Full release acceptance** also requires independently
administered mandatory reviews/checks, a blocked bypass experiment, independent
build/trust-root review, real authorized ARM/USB execution, consent and privacy,
child protection, accessible documents, indigenous/local cultural autonomy and
license review. These have no inherited PASS.

Rollback: keep predecessor main/PR SHAs and manifests, no force update and no
deletion. The S16 source branch remains draft and without merge until human
approval, irrespective of CI outcome. `claim_allowed=false`, `promotion=false`,
`TOKEN_VAZIO != 0`. Reject all unintended physical/device writes by default.
