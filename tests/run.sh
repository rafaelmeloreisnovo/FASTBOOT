#!/bin/sh
set -eu
mkdir -p build
CC=${CC:-cc}
"$CC" -std=c11 -O2 -Wall -Wextra -Werror -Iinclude src/fb_omega.c tests/test_fb.c -o build/test_fb
./build/test_fb
"$CC" -std=gnu11 -O2 -Wall -Wextra -Werror -Iinclude src/fb_omega.c host/linux_usbfs.c -o build/fb-linux
"$CC" -std=c11 -O2 -ffreestanding -fno-builtin -nostdlib -Iinclude -c src/fb_omega.c -o build/fb_l0.o
if [ -n "$(nm -u build/fb_l0.o)" ]; then
  echo "FAIL: L0 has undefined symbols" >&2
  exit 1
fi
echo "PASS: Linux adapter builds and L0 object has no undefined symbols"
