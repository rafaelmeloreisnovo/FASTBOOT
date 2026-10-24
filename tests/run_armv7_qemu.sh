#!/bin/sh
# ARMv7 user-mode QEMU: validates ARM ELF instructions, NOT physical ARM32.
# Host/test tools may use libc; ASM source never links external runtime calls.
set -eu
: "${EXPECTED_HEAD:?exact source HEAD SHA required}"
test "$(git rev-parse --verify HEAD)" = "$EXPECTED_HEAD" || {
  echo 'FAIL: source SHA does not match expectation' >&2; exit 66;
}
test "${#EXPECTED_HEAD}" -eq 40 || exit 65
for t in arm-linux-gnueabihf-gcc qemu-arm readelf sha256sum; do
  command -v "$t" >/dev/null || { echo "NOT_RUN: missing test-only tool $t" >&2; exit 77; }
done
mkdir -p build/armv7-qemu
for name in getvar classify session; do
  bin="build/armv7-qemu/test_$name"
  arm-linux-gnueabihf-gcc -std=c11 -O2 -marm -march=armv7-a -mfpu=vfpv3-d16 \
    -Wall -Wextra -Werror -Iinclude \
    src/fb_omega.c "tests/test_asm_$name.c" "asm/armv7/fb_$name.S" \
    -Wl,-z,noexecstack -o "$bin"
  readelf -h "$bin" | grep -Eq 'Class:[[:space:]]+ELF32' || exit 68
  readelf -h "$bin" | grep -Eq 'Machine:[[:space:]]+ARM' || exit 68
  qemu-arm -L /usr/arm-linux-gnueabihf "$bin"
  sha256sum "$bin"
done
echo "PASS_SCOPED=EMULATED_ARMv7_USERSPACE, SOURCE_HEAD=$EXPECTED_HEAD"
echo 'NOT_RUN=PHYSICAL_ARM32,ANDROID_PHONE,USB_OTG,BAREMETAL_SOC'
echo 'claim_allowed=false; promotion=false'
