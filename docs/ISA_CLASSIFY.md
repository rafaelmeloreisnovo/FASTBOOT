# FASTBOOT Ω — response classification in direct ISA

## Source, boundary, and scope

This is a successor to ISA_DIRECT.md; no deletion or replacement of the older
C11 parser, guard, Android source or Linux usbfs adapter. Written as freestanding
assembler modules: armv7 AAPCS (ARM mode), aarch64 AAPCS64 and x86-64 SysV.

Entry point: `fb_asm_classify_reply(wire, n, data_size_out)`.

- Return kind in ABI integer result register: FB_BAD=0, FB_OKAY=1,
  FB_FAIL=2, FB_INFO=3, FB_DATA=4, FB_TEXT=5.
- Range for n: **4..256 inclusive**. Inputs are raw bytes, no NUL terminator
  assumed, no allocations.
- Recognize exact four-byte ASCII prefix OKAY, FAIL, INFO, TEXT, DATA.
- DATA must contain exactly 12 bytes, and 8 ASCII hexadecimal digits,
  decoding a 32-bit size. It classifies but **never transfers DATA**.
- On success store `data_size_out=0` except DATA, which stores decoded size.
- On FB_BAD, `data_size_out` is not modified.
- The output pointer must address naturally aligned writable uint32 memory,
  distinct from the input payload; input pointer must address n readable bytes.
- Payload (especially TEXT) is not interpreted, normalized or printed by
  these routines. Any display requires separate review and encoding treatment.

ABI register map:
ARMv7: r0 wire / r1 n / r2 output, r0 return; scratch r3 r12 and r1 after
bounds verified; no callee-saved registers modified.
AArch64: x0 wire / x1 n / x2 output, w0 return; scratch x3 x4 x5.
x86-64: rdi wire / rsi n / rdx output, eax return; scratch rcx r8 r9 r10.
No runtime libc, heap, stack allocation, system call, JNI, ART, DEX, SDK,
Gradle, NDK, R8 or third-party code inside Assembly.

This recognizes a host-driven Fastboot response; a USB bus host controller
and transfer layer are separate, requiring real hardware documentation.
The command whitelist remains getvar only; no flash/erase/download/unlock.
Any received DATA must still be BLOCKED by the upstream session gate.

## Local evidence (host, not target)

`sh tests/run_classify_asm.sh` compiled and ran on x86-64:
101,580 differential byte/length test vectors matched `fb_parse` with
the decoded DATA size and output untouched on invalid packets;
three pointer-null negative tests also passed. Clang ASan+UBSan host run PASS.
The prior `sh tests/run_asm.sh` getvar test still passed 14,848 vectors.

ARMv7: object ELF32, little-endian, `.text` 448 bytes, SHA256
`3e0c0a7153b56d53227307c5c9e2b9076edc092e0b4adf14a5c9563835d626fa`.
AArch64: object ELF64, little-endian, `.text` 424 bytes, SHA256
`9c2a853e7a348ecaa04756c3c260be82addc97486cdb55ea05a1983880a5ddd4`.
Both checked with `nm -u` (empty) and `readelf -r` (no relocations).
ISA-mismatch negative compilation rejected. Neither ARM binary was
executed on physical ARM hardware; compilation is NOT execution.

Scientific/control references:
Fastboot AOSP README:
https://chromium.googlesource.com/aosp/platform/system/core/+/refs/heads/upstream/fastboot/
Legacy protocol:
https://android.googlesource.com/platform/system/core/+/refs/tags/android-cts-7.0_r33/fastboot/fastboot_protocol.txt

## Gates and custody

SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM; TOKEN_VAZIO != 0.
TOKEN_VAZIO_PHYSICAL_ARM, TOKEN_VAZIO_USB_OTG,
TOKEN_VAZIO_BAREMETAL_MMIO_SOC, TOKEN_VAZIO_PROVIDER_CI,
TOKEN_VAZIO_INDEPENDENT_REVIEW. claim_allowed=false, promotion=false,
rollback=all prior branch commits and main preserved.
