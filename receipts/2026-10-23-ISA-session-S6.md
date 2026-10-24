# FASTBOOT Ω — S6 source successor, not promotion
GitHub private rafaelmeloreisnovo/FASTBOOT PR #1, predecessor
d10bb01be9ddc0192fda92abe25587d2b663c67d. New HEAD must be
verified after commit; do not invent. Source: 3 ISA session .S files,
host-only differential test and assembly build gate, additive test script.
Local x86-64 101800 differential assertions vs C + boundary tests PASS;
Clang ASan+UBSan PASS; old S4/S5 test chain rerun PASS.
ARMv7 148 B SHA256 40f78873493cbd94b2b890f91adcb61f83c35c4966154c82735229e4216a09aa;
AArch64 140 B SHA256 0839949dc5a060236d2ced755d36fdf915814ab8274d9542bf18759a0c6d0875;
nm -u empty, readelf no relocs, no ARM physical execution.
Fail-closed invalid state, MAX 32 INFO/TEXT, DATA blocked, no retries.
TOKEN_VAZIO=USB_PHY, ARM_RUNTIME, SOC_MMIO, real failover/failback, provider
CI, JNI/APK, independent review. claim_allowed=false; no merge.
Rollback=previous branch commit and original main unchanged.
