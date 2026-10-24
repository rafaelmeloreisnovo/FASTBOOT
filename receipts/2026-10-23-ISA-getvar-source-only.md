# RAFAELIA FASTBOOT Ω — ISA-direct successor receipt

Parent PR #1 HEAD before this delta:
924307bde8ad9f2a4e37e1409fdab1c36e8c5678.
Changes: Assembly pure getvar encoder in ARMv7, AArch64, x86-64;
host-only equivalence tests; standalone opt-in ISA build gate; docs.
No replacement of L0 parser, guard or adapters, no main modification.
Local x86-64 executed 14848 byte-vector comparisons PASS plus four
invalid API cases; host AddressSanitizer/UBSan PASS.
ARMv7a-none-eabi and aarch64-none-elf compiled objects without
undefined symbols/relocations, raw text 224/212 bytes and hashes in
docs/ISA_DIRECT.md; no physical ARM execution.
TOKEN_VAZIO: exact new Git HEAD until readback; CI; ARM physical runtime;
USB OTG; bare-metal SoC USB MMIO; Mi Assistant; JNI/APK intentionally
not in L0. claim_allowed=false; promotion=false.
