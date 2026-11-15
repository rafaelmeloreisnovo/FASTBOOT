# FASTBOOT Agent Operating Contract

Version: 1.0
Scope: Moto E7 Power Android host and Redmi 12 Fastboot target.

## Agent roles and order

1. Invariante das Invariantes: preserve identity, source, artifact, execution, evidence, claim, predecessor, unknown and rollback distinctions.
2. Evoluidor de Sistema: freeze baseline, state H1/H0, choose the smallest reversible delta, and define acceptance and rollback.
3. RAFAELIA Evidence Gate: classify claims; block promotion when source, environment, test, hash, falsifier or requirement is missing.
4. Verificador de Grafos: check typed nodes/edges, orphan dependencies, contradictory refs, cycles, identity collisions and evidential support.
5. RAFAELIA Phase Integration: map L0 core, L1 Android USB transport, L2 Java/application, package/build and physical operation.
6. RAFAELIA Ω Orchestrator: coordinate only selected roles, keep one producer authoritative and stop at acceptance or a blocker.
7. Rio Invisível: trace intent → USB permission → interface → endpoint I/O → parser → report → receipt; identify inference and data loss.
8. Escriba Hebreu: normalize identifiers and separate literal values, aliases, observations, hypotheses and unknowns.
9. Roteador de Mestres: choose the smallest applicable role set and route contradictions to their source owner.
10. Mestre Zen: reduce unresolved issues to the next discriminating observation. Metaphor is not a test.

This sequence is a procedure, not a claim that agents ran autonomously. Record a role as APPLIED only with its check and resulting artifact/evidence; otherwise NOT_RUN.

## Current system boundary

- Host: Motorola Moto E7 Power, Android 10, reported ABI armeabi-v7a.
- Target: Redmi 12 model 23053RN02A; expected Fastboot product fire remains a hypothesis until observed from the physical target.
- USB: Android UsbManager permission, followed by unambiguous interface and bulk endpoints.
- Allowed protocol: bounded getvar reads.
- Disallowed in this project profile: flash, erase, unlock, boot, reboot, partition writes, exploit paths and undocumented OEM protocol.
- Android version strings and settings screenshots do not establish a HyperOS build through Fastboot. Keep TOKEN_VAZIO_FASTBOOT_ANDROID_VERSION_DOES_NOT_PROVE_HYPEROS_BUILD until independently observed.
- Offline profile: no INTERNET permission, dynamic code download, remote command channel, serial upload or descriptor upload.

## Evidence/status vocabulary

- Execution: NOT_RUN | RUNNING | DONE | FAILED.
- Evidence: NARRATIVE | DOCUMENTED | SIMULATED | ESTIMATED | EXTRAPOLATED | MEASURED_LOCAL | REPRODUCED | CROSS_HOST | PROVEN | INVALIDATED.
- Claim: ALLOWED_SCOPED | BLOCKED.
- Missing evidence: TOKEN_VAZIO_<field>.

Rules: SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM. IMPLEMENTED_UNTESTED != PASS. A green workflow proves only its named checks on its recorded ref. An expected authority NO-GO remains a recorded result. User screenshot/transcription is DOCUMENTED_USER_OBSERVED, not an independent hardware measurement. Never invent hashes, device results, permissions, validations or review.

## Node.js 20 / validate-check triage

Before changing workflow configuration, identify the current PR/head and latest checks. For each Node.js 20 error:

1. Record workflow, run, job, step, exact commit SHA, full error and whether it used PR merge ref or branch head.
2. Inspect the workflow's node-version/runtime declaration and the pinned action versions.
3. Verify action/runtime support from primary project sources; an error summary alone does not establish that Node.js 20 was removed.
4. Distinguish hosted-runner/runtime policy, action incompatibility, project dependency failure and unrelated gate failure.
5. Reproduce only if the necessary runtime/toolchain is available.
6. Make the smallest fix, preserving the failed run and adding a successor receipt.
7. Verify checks on the successor SHA before changing claim status.

Run the narrowest requested validation and leave unrelated checks TOKEN_VAZIO.

## Graph and data-flow contracts

Use typed edges: implements, uses, tests, observes, derived_from, supports, contradicts, supersedes, rollback_of. Unsupported proposed edges are HYPOTHESIS_EDGE.

USB chain: user action → permission granted → device/interface selected → unique IN/OUT endpoints → bounded getvar write → bounded response → parser result → target classification → receipt. A missing edge blocks downstream claims. Consider denial, timeout, disconnect, malformed descriptors, duplicate endpoints, unexpected replies and wrong product as falsifiers.

## Change and rollback

- Record baseline SHA and current provider status.
- Preserve predecessor; use successor commits/PRs.
- Define positive and negative acceptance criteria before behavior changes.
- Keep changes isolated and reviewable.
- On failure, classify it and stop dependent promotion.
- Roll back via reviewed revert/successor; never rewrite or delete historical evidence.
- Update pointers only after source and receipt readback.

## Receipt format

μID | timestamp | producer/ref | parent | kind | delta | execution | evidence | falsifiers | TOKEN_VAZIO gaps | rollback | next

Use stable ASCII identifiers for schemas/device matching. Unicode labels are display text, not keys.

## Known state at authoring

- PR #8 was reported merged to main at ad08aec52684b298ceb31c5b25e62bb8af2359b6.
- PR #9 is a draft for assistant-source hardening and validation.
- On recorded PR #9 SHA a35ad7d5cc5c7146e72202aa56d9e8c82d68f345, Android source-contract and ISA workflows passed; S16 build workers and artifact comparison passed; its authority/physical-evidence NO-GO step failed closed.
- Those results apply only to that SHA. Newer commits require fresh readback.
- APK/DEX/ELF/JNI installation, Moto E7 USB Host, Redmi 12 descriptors and physical getvar:product remain TOKEN_VAZIO until observed.

## Closeout

Report F_ok (scoped evidence), F_gap (typed unknowns), F_next (one reversible discriminating action). Keep claim_allowed=false unless its evidence contract is satisfied.