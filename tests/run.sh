#!/bin/sh
set -eu
mkdir -p build
CC=${CC:-cc}
WARN="-Wall -Wextra -Werror -Wshadow -Wconversion -Wstrict-prototypes -Wmissing-prototypes"
"$CC" -std=c11 -O2 $WARN -Iinclude src/fb_omega.c tests/test_fb.c -o build/test_fb
./build/test_fb
"$CC" -std=c11 -O2 $WARN -Iinclude src/fb_omega.c src/fb_guard.c tests/test_guard.c -o build/test_guard
./build/test_guard
"$CC" -std=gnu11 -O2 $WARN -Iinclude src/fb_omega.c src/fb_guard.c src/fb_usb_desc.c host/linux_usbfs.c -o build/fb-linux
"$CC" -std=c11 -O2 $WARN -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -Iinclude -c src/fb_omega.c -o build/fb_omega.o
"$CC" -std=c11 -O2 $WARN -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -Iinclude -c src/fb_guard.c -o build/fb_guard.o
"$CC" -nostdlib -r build/fb_omega.o build/fb_guard.o -o build/fb_l0_combined.o
test -z "$(nm -u build/fb_l0_combined.o)" || { echo "FAIL: L0 has external undefined symbols" >&2; exit 1; }
if "$CC" -std=c11 -Iinclude -DFB_OMEGA_ENABLE_WRITE=1 -c src/fb_guard.c -o build/forbidden_write.o >/dev/null 2>&1; then
  echo "FAIL: forbidden write profile compiled" >&2
  exit 1
fi
echo "PASS: L0 no undefined symbols; forbidden write profile rejected; Linux host builds"

# S9 bounded pure-C USB descriptor scanner tests, no device connected.
sh tests/run_usb_desc.sh

# S10 fail-closed descriptor integrity and mock no-TX gates (no physical device).
sh tests/run_usb_desc_s10.sh

# S13 fail-closed privacy and USB-path host regression, wholly mocked.
sh tests/run_privacy_s12.sh
