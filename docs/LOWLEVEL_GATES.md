# FASTBOOT Ω — L0 low-level gates and honest hardware boundary

SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM. TOKEN_VAZIO != 0.

## Hardware boundary
The freestanding **src/fb_omega.c + src/fb_guard.c** does not access Linux, Java,
libc, Android, file descriptors, syscalls, USB drivers, heap, clock or dynamic memory.
Its buffer parsing/state transitions compile to CPU instructions. A compiler cannot
invent the USB host controller MMIO register map, PHY power/reset sequencing,
DMA setup, interrupts, clocks, SoC-specific IOMMU mapping, cable role or permission.
Real USB requires either a separately scoped host interface (Linux usbfs,
Android UsbManager, OS dependent) or a vendor-specific bare-metal controller
driver after obtaining reliable silicon documentation and physical tests.
No imaginary address constants, no unverified inline ASM, no general bare-metal
USB support claim.

## Stage sequence
External interface verification + user permission -> USB claim -> L0 guard BIND ->
getvar-only bulk OUT -> TX_COMPLETE -> bounded bulk IN/INFO/TEXT -> OKAY/FAIL.
L0 permits no DATA transfer. Timeout/IO failure/invalid transition/unknown replies
cause quarantine; device is not retried automatically. On USB close, transition
CLOSED; **failover/failback is only a host procedure** after physical release, new
device identification and fresh permission, never silent hot-switching.

## Watchdogs
- Protocol INFO/TEXT limit: 32 responses.
- Host USB transfer timeout: 2500ms per transfer.
- Session budget: 12000ms of *host-measured elapsed monotonic time* supplied to L0;
  L0 has no timer or interrupt by itself.
- Wall-clock scheduling jitter and USB driver block behavior are NOT proven.
- On success/failure, release USB interface/connection. No flash, erase, unlock or
  download implementation; no automatic retransmission and no unlocking workaround.

## Compile gates
C11 freestanding `-ffreestanding -fno-builtin -nostdlib -fno-stack-protector`.
`-Wall -Wextra -Werror -Wshadow -Wconversion -Wstrict-prototypes
-Wmissing-prototypes`. The `-Wshadow` warning prevents shadow identifiers; it
is not possible to compile C without identifiers. C comments alone do not
alter compiler options; `#if/#error` acts at preprocessing time, and
`-DFB_OMEGA_ENABLE_WRITE=1` fails by design.
L0 has `void` entrypoints and pointer-to-buffer operations. Testing uses
host libc, explicitly OUTSIDE L0. Original source is retained; no opaque
binary or vendor code copied.

## Evidence separation
Local synthetic: 7,204 checks in guard test plus existing parser test.
Host `gcc` + `clang` sanitizer tests and ARMv7/AArch64 **object** builds.
This is not a real ARM physical run, Android APK, notebook USB proof or
provider CI run. Source pipeline: `sh tests/run.sh`.
No self-authorized promotion. Review and hardware checks required.
