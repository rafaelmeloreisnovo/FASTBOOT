# FASTBOOT Ω / S8 — isolated CPU instruction modules

This is a scoped *candidate*, not an approval. S4/S5/S6 are preserved.

- P0: each ASM module is self-contained with documented ABI, bounded buffer and no external include/import.
- P1: an ELF ET_REL has the expected ISA, no undefined symbols, relocations, dynamic loader or allocated non-text sections.
- P2: extract individually checked .text; concatenate raw bytes and record entry offsets. No linker-added alignment traps.
- P3: local differential tests against the frozen predecessor and negative gate-injection tests.
- P4: actual ARM/USB hardware observations require independent receipts. Cross assembly is NOT real ARM execution.

Real dependencies: the CPU ISA, executable memory, a valid caller and ABI. USB host or Android integration requires independently proven platform/hardware facilities. Build tools are build-time only, never an ELF runtime dependency. Do not claim bare-metal USB access without controller evidence. Authorization and provenance reviewed before promotion.

SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM. TOKEN_VAZIO != 0. claim_allowed=false. promotion=false. PR draft. main unchanged.