# FASTBOOT Ω S9 — independent source, verified USB binding, native ARM gate

Origin: merged PR #1 main @ a0faaccef7ba55747cd32cefd886a07723510d9a.
Successor S9 is additive in a separate DRAFT PR; do not auto-merge.

## Boundary 0: instruction-only code

Previous S4–S6 Assembly units remain byte-for-byte untouched. The S8
per-ELF gates remain unchanged. New src/fb_usb_desc.c is an original
freestanding C11 bounded USB *descriptor validator*, not a USB driver:
no libc/syscall/heap, no global state; compiled for ARMv7 and AArch64
with zero undefined symbols. Host tests are external test-only code.

The validator requires a 9-byte USB configuration header, exact
wTotalLength <=1024, a unique interface alternate setting zero with
class 0xff, subclass 0x42 and protocol 0x03, exactly two BULK endpoint
descriptors, nonzero max-packet lengths, and exact OUT/IN addresses.
It rejects ambiguity, truncation and incorrect endpoints.

## Boundary 1: Linux USB L1 adapter (necessarily OS-dependent)

Before USBDEVFS_CLAIMINTERFACE, the Linux host reads the *active*
configuration through USB standard GET_CONFIGURATION, reads the device
descriptor and at most four configurations with GET_DESCRIPTOR, then
passes only the active descriptor bytes into the standalone validator.
Unsupported/ambiguous descriptors fail-closed; no claim, no bulk TX.
The adapter still requires kernel USB permissions and POSIX ioctl and
is *not* a freestanding USB driver. This gate is compiled and unit
tested, but not proven on an attached physical device.

No flash, download, erase, unlock, reboot or automatic failover.

## Boundary 2: ARM native eligibility

The S7B candidate is now versioned as tests/run_native_arm.sh and
tests/check_native_arm_negative.sh. It checks exact HEAD and that its
own provenance scripts are committed, selecting ARMv7 or AArch64 ABI
from the native compiler and returning NOT_RUN=77 on non-ARM hosts.
Local x86 negative test passed. Physical ARM tests were NOT_RUN.
The kernel uname test is not attestation of real silicon: emulation
is possible; native userland PASS is never a USB or silicon claim.

## Gates / evidence and constraints

Locally scoped: S4 14,848; S5 101,580; S6 101,800 differential
vectors; nine original ASM source objects audited by S8; additional
S9 20,199 USB descriptor mutation/fuzz checks, GCC+Clang sanitized
run and ARMv7/AArch64 freestanding compile (undefined externs zero).
Linux host C compiled with strict warnings. These local comparison
artifacts were assembled from previous local snapshots: not identical
GitHub source claims until individual Git blob SHA reconciliation.

GitHub Actions prior runs had runner_id=0, steps=[], FAIL before
observable execution. No code-level hotfix is justified for an
undisclosed provider-side failure. No CI PASS, no physical ARM, USB,
OTG, JNI, NDK or standalone bare-metal SoC MMIO test claim.

Human review + independent exact-head build evidence required before
promoting. SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM.
TOKEN_VAZIO != 0; claim_allowed=false; promotion=false.
Rollback is the exact main merge commit, predecessor branch preserved.


## S9.1 compiler metadata isolation

Clang ARMv7 emitted original .ARM.exidx unwind metadata with one
R_ARM_PREL31 relocation referring to the .text section. The original
ELF object is **retained**. The gate generates a separate derived ELF
with only the unwind metadata sections removed, verifies there are no
remaining relocations, then extracts .text into a plain instruction
binary. No external symbols were present in either object. AArch64
already had no relocations. No claim of zero relocations in the raw
ARMv7 compiler output is permitted. The stripped derivative is not a
full bootable firmware image and does not prove hardware execution.
