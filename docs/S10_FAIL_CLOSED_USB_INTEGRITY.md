# FASTBOOT Ω S10 — last observed safe boundary: consistency and post-claim USB gate

## Reproduced defect / predecessor proof

Before S10, `src/fb_usb_desc.c` accepted a 32-byte descriptor whose
`bNumInterfaces=2` although only one alt=0 interface descriptor existed.
Observed predecessor returned 1 (unsafe acceptance). S10 returns 0 and
zeroes the output binding, even when the caller supplies valid-looking
FF/42/03 class and two BULK endpoints.

## S10 freestanding L0

The original C11 scanner (not a Linux driver) now counts each distinct
alternate-setting-zero interface, rejects duplicate alt=0 descriptors,
rejects orphan alternate settings and insists the observed count equals
`bNumInterfaces`. Bounds remain 9..1024 descriptor bytes. Each previous
descriptor is independently bounded before being rescanned. No libc,
heap, global state, OS API, syscall or USB IO. Complexity O(n²) but strictly
bounded by 1024 input bytes. The compiler's use of stack/registers is
not in itself an external runtime dependency.

Local test: 120,007 boundary/malformed/alternate/fuzz cases PASS, plus
the predecessor's 20,199 cases PASS and ASan+UBSan PASS. Source code
was compared by exact Git blob SHA to the uploaded, tested local file.

## S10 host-side L1 (Linux dependent by design)

After validating the active configuration *before* claiming an interface,
the adapter initializes its guard, claims the interface, reads the
active configuration again and sends standard GET_INTERFACE to verify
alternate setting zero. Any mismatch, stall or configuration change
fails closed and releases a claimed interface; no Fastboot BULK OUT.
The remaining window between checking and actual transmission is a
physical race not wholly eliminable by prechecks.

Five mock-kernel test scenarios (no real USB I/O): valid transaction
succeeds, alt=1 rejected, GET_INTERFACE stall rejected, active
configuration changing after claim rejected, inconsistent
bNumInterfaces rejected before claim. For four negative scenarios
BULK OUT count is zero; claimed interfaces are released.

## Target proofs / limits

Clang compiled freestanding ARMv7 and AArch64 L0 objects, `nm -u`
empty. Original ARMv7 contains a self-relocation in `.ARM.exidx`,
which is preserved for audit; a separately derived text-only ELF
contains zero relocations. New text-only binary sizes: ARMv7 880 B,
AArch64 724 B, **not firmware, APK or native silicon execution**.
S4/S5/S6 predecessor synthetic suites (14,848 / 101,580 / 101,800
vectors) and S8 strict ELF/negative gates were rerun locally on
historical ASM source snapshots; do not inherit PASS for all live files
without exact-source/CI evidence.

GitHub Actions runner failures remain unresolved; no physical USB/OTG
or ARM32/64 userland test. L1 Linux needs a kernel, usbfs and permissions;
bare-metal USB still needs verified SoC controller MMIO/PHY support.
An always-zero external-dependency USB device host across every SoC
is not a coherent portability claim.

## Custody

S10 is a successor to S9.1 on the same isolated branch. Never rewrite
the merged S8 main, the S9.1 predecessor, or authored upstream source.
No flash, unlock, download, erase, reboot, retry/failover or automatic
merge. SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM,
TOKEN_VAZIO != 0; claim_allowed=false; promotion=false.
