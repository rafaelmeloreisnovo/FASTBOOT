# FASTBOOT Ω — S14: prior art, evidence gates and merge-state reconciliation
Date: 2026-10-24. Classification: engineering research notebook only, NOT a published scientific paper or independent certification.

## Observed GitHub transition
- PR #4 was previously observed as draft/unmerged at HEAD `d3abeaeb930bc827d70fcdbab04d62a2f921a036`.
- A subsequent provider read returned **merged=true**, merged_at=`2026-10-24T20:29:38Z`, merge SHA `c046bb1ba29fbf4445b7df82b3a59dff907b5494`; main matches this SHA. This is a state transition; actor, authorization and independent review remain unverified.
- At the follow-up read main remained **protected=false** and repository rulesets returned **[]**; GitHub's detailed branch protection API had previously returned 403 to this integration. Those observations do not establish why the merge was allowed.
- GitHub PR review submissions returned **[]**; this is not proof that no out-of-band human review ever happened.
- The prior CI run [#40129145151](https://github.com/rafaelmeloreisnovo/FASTBOOT/actions/runs/40129145151) reported 3/3 PASS on the PR source SHA. The PR merge SHA had **no PR-triggered workflow runs returned by the queried endpoint**. This is narrower than 'no CI exists anywhere'.
- **Do not silently rewrite S13B historical receipts**. Append this correction and classify `MERGE_REVIEW_NOT_OBSERVED` and `MERGE_SHA_CI_NOT_OBSERVED` separately.
- No rollback/merge/revert/force/delete/firmware action performed by this S14 research branch.

## Cross-checked scholarly background (external authors' work)
1. [Reproducible Builds and Insights from an Independent Verifier for Arch Linux](https://www.alphaxiv.org/abs/2505.21642) — Independent rebuilds can identify source-to-binary discrepancies, but identical binaries do not establish a trustworthy compiler. **Gate:** `G14-02`.
2. [USBIPS Framework: Protecting Hosts from Malicious USB Peripherals](https://www.alphaxiv.org/abs/2409.12850) — Cross-layer malicious-peripheral attacks exceed simple interface/path allowlists; evaluated solution targets Windows, not this Linux Fastboot host. **Gate:** `G14-03`.
3. [Kettle: Attested Builds for Verifiable Software Provenance](https://www.alphaxiv.org/abs/2605.08363) — TEE-backed attestation binds source/toolchain/environment to artifact digests; it does not prove code safety or compiler semantic correctness. **Gate:** `G14-04`.
4. [From Agent Traces to Trust: A Survey of Evidence Tracing and Execution Provenance in LLM Agents](https://www.alphaxiv.org/abs/2606.04990) — Typed execution provenance and evidence traces can show which observation supports which claim; final output alone is insufficient. **Gate:** `G14-05`.

The papers are **prior-art context**, not proof of this project's novelty, safety or validity. We read relevant pages; we did not reproduce their experiments, audit their full datasets, or import/copy their implementations.

## Falsifiable acceptance protocol
- **G14-01 / MERGE_REVIEW_NOT_OBSERVED / P0: OBSERVED_GAP** — Acceptance: Independent reviewer approval and enforced required checks for a subsequent candidate, with a provider-observed rejected unapproved merge. Failure/rollback: No automated revert or delete; preserve the merged SHA and raise explicit governance correction.
- **G14-02 / INDEPENDENT_REBUILD_NOT_RUN / P1: NOT_RUN** — Acceptance: Two independently provisioned builders produce byte-identical outputs for the same source HEAD and disclosed toolchains; report differing trust roots. Failure/rollback: Do not distribute unverified binary; retain all build receipts.
- **G14-03 / PHYSICAL_USB_IDENTITY_UNVERIFIED / P0: UNVERIFIED** — Acceptance: With explicit consent on sacrificial hardware, verify device descriptor stability and fail-closed behavior under reconnect/race tests; no flash/unlock/erase. Failure/rollback: Disconnect transport safely and retain host-only mode.
- **G14-04 / ATTESTATION_NOT_RUN / P1: NOT_RUN** — Acceptance: Independently verify build provenance bound to source, toolchain and artifact hashes or label as unauthenticated CI log. Failure/rollback: Preserve provenance statements as untrusted and stop promotion.
- **G14-05 / CLAIM_TRACE_PARTIAL / P1: UNVERIFIED** — Acceptance: Every release claim has an exact SHA, execution environment, observable log and explicit scope; missing evidence stays a typed nonzero gap. Failure/rollback: Demote claim to hypothesis, without deleting historical records.
- **G14-06 / MERGE_SHA_CI_NOT_OBSERVED / P0: NOT_RUN** — Acceptance: A CI record on exact merge SHA or independent binary equivalence receipt; do not inherit parent-head PASS blindly. Failure/rollback: No production release on unverified merge artifact.
- **G14-07 / RESEARCH_NOVELTY_UNVERIFIED / P1: UNVERIFIED** — Acceptance: Complete prior-art review and independent scientific review before claims of invention, theorem or academic validation. Failure/rollback: Use 'engineering study' status and preserve source authorship.
- **G14-08 / CHILD_PRIVACY_CULTURAL_REVIEW_NOT_RUN / P1: NOT_RUN** — Acceptance: Human review of logging/consent/accessibility/child-protection and respect for local cultures before broad deployment. Failure/rollback: Retain private, minimal data collection and no external sharing.

## Boundary and custody
- L0 freestanding C/ASM is independent of L1 Linux usbfs kernel calls; being dependency-free in L0 does not make an end-to-end USB stack independent of hardware/host OS.
- `SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM`; `TOKEN_VAZIO != 0`.
- Physical ARM32/64 phones and USB OTG are **NOT_RUN**. The literature repository in alphaXiv is a **private bookmark folder**, not a paper submission or endorsement.
- Claim and production promotion remain **false** until exact-commit evidence, enforced governance and human/physical acceptance; protect minors, privacy and local cultural autonomy.

For machine-readable gates, see `docs/research/FASTBOOT_S14_EVIDENCE_GATES.json`. All predecessor source, commits and receipts are retained.
