#!/bin/sh
set -eu
mkdir -p build
CC=${CC:-cc}
CLANG=${CLANG:-clang}
"$CC" -std=c11 -O2 -Wall -Wextra -Werror -Wshadow -Wconversion -Iinclude \
 src/fb_usb_desc.c tests/test_usb_desc.c -o build/test_usb_desc
./build/test_usb_desc
"$CLANG" -std=c11 -O2 -target armv7a-none-eabi -ffreestanding -fno-builtin \
 -fno-stack-protector -Wall -Wextra -Werror -Wshadow -Wconversion -Iinclude \
 -c src/fb_usb_desc.c -o build/fb_usb_desc_armv7.o
"$CLANG" -std=c11 -O2 -target aarch64-none-elf -ffreestanding -fno-builtin \
 -fno-stack-protector -Wall -Wextra -Werror -Wshadow -Wconversion -Iinclude \
 -c src/fb_usb_desc.c -o build/fb_usb_desc_aarch64.o
for arch in armv7 aarch64; do
  if [ -n "$(nm -u build/fb_usb_desc_${arch}.o)" ]; then echo "FAIL unresolved $arch" >&2;exit 1;fi
done
echo 'PASS: pure low-level descriptor decoder with no unresolved ARM symbols (no physical USB claims)'
