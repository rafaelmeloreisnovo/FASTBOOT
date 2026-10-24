# FASTBOOT Ω S6 — direct ISA session state machine

Provenance: predecessor PR #1 @ d10bb01be9ddc0192fda92abe25587d2b663c67d.
No deletion/overwrite of earlier C11 parser, watchdog, getvar and classifier or USB adapters.

`fb_asm_session_step`: ARMv7 AAPCS (r0 session ptr, r1 kind -> r0 state),
AArch64 AAPCS64 (x0 session ptr, w1 kind -> w0 state), x86-64 SysV
(rdi session ptr, esi kind -> eax state). Two naturally aligned writable
uint32 words at +0 (state) and +4 (INFO/TEXT count), otherwise caller contract.
No includes, stack manipulation, heap, C runtime, JNI/ART/DEX/SDK/NDK,
syscalls, privileged instructions, controller MMIO, or external references.
Branching and constant bounded counting are real ISA instructions.

States: 0 WAIT, 1 DONE, 2 DEVICE_FAIL, 3 BLOCK_DATA,
4 PROTOCOL_ERROR. Kinds: 1 OKAY, 2 FAIL, 3 INFO, 4 DATA, 5 TEXT.
Wait + INFO/TEXT increments count through 32 inclusive, then protocol
error on further message; OKAY/FAIL/DATA are terminal. Terminal
states never reopen on subsequent events, and corrupt state > 4
transitions to protocol error. Null state pointer fails closed.

Observed host x86-64 local: 101,800 synthetic differential vectors vs
preserved C `fb_session_accept` PASS; additional guard checks covering
32 INFO limit, malformed state, null pointer and DATA terminal block.
ASan+UBSan passed. Existing `sh tests/run_asm.sh` S4/S5 chain reexecuted.
Clang produced ARMv7 ELF32 `.text` 148 bytes, SHA256
40f78873493cbd94b2b890f91adcb61f83c35c4966154c82735229e4216a09aa;
AArch64 ELF64 `.text` 140 bytes, SHA256
0839949dc5a060236d2ced755d36fdf915814ab8274d9542bf18759a0c6d0875.
Both `nm -u` empty, `readelf -r` no relocs; wrong ISA compilation
rejected. No actual ARM CPU/device tests.

Host-only harness uses libc for comparison; this does not make the ISA
modules dependent on libc. Binaries are routines and not independently
bootable or USB controllers. Physical Fastboot requires a separately
proven USB host transport/driver and device permission; platform controller
MMIO maps and PHY are still unknown for bare metal.
No automatic USB failover/failback and no flash/erase/unlock/download/reboot.

R3: F_ok=SOURCE + local test evidence; F_gap=ARM physical execution,
USB controller, remote CI, independent review; F_next=exact-head CI and
hardware-specific ARM/OTG tests. SOURCE != ARTIFACT != EXECUTION != EVIDENCE
!= CLAIM. TOKEN_VAZIO != 0. claim_allowed=false; promotion=false.
