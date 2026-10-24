# FASTBOOT Ω — ISA reply classify successor: SOURCE_ONLY

Date: 2026-10-23, America/Sao_Paulo.
Parent source: GitHub rafaelmeloreisnovo/FASTBOOT PR #1
a85a18481e13e13aacc2b11712e5ce3d7abd570f (draft, unmerged).
Added: asm/*/fb_classify.S, differential host harness, isolated assembly gate,
docs/ISA_CLASSIFY.md and append-only receipt; tests/run_asm.sh appends new
classifier test without removing older getvar test.
Local evidence: 101580 vectors+3 invalid pointer tests PASS, clang
ASan/UBSan PASS, prior 14848 getvar equivalence vectors PASS.
ARMv7 448 B SHA256 3e0c0a7153b56d53227307c5c9e2b9076edc092e0b4adf14a5c9563835d626fa
AArch64 424 B SHA256 9c2a853e7a348ecaa04756c3c260be82addc97486cdb55ea05a1983880a5ddd4.
Both no extern undefined and no relocations; real ARM not run.
Failure policy: no disk/firmware write path, invalid responses rejected,
DATA classified but remains blocked by previous guard; no automatic retry.
Exact new GitHub HEAD to be confirmed by remote readback.
TOKEN_VAZIO: physical USB/OTG, real ARM ABI execution, device controller
MMIO, Android binary/app, CI on exact HEAD, independent review.
claim_allowed=false; promotion=false; rollback=parent retained/main unchanged.
