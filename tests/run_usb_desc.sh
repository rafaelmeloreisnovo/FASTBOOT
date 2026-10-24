#!/bin/sh
set -eu
mkdir -p build
CC=${CC:-cc}
CLANG=${CLANG:-clang}
OBJCOPY=${OBJCOPY:-llvm-objcopy}
READELF=${READELF:-readelf}
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
  src="build/fb_usb_desc_${arch}.o"
  clean="build/fb_usb_desc_${arch}_textonly.o"
  if [ -n "$(nm -u "$src")" ]; then echo "FAIL unresolved $arch" >&2;exit 1;fi
  if [ "$arch" = armv7 ]; then
    # ARM EHABI exidx is compiler metadata, not part of the CPU function.
    # Preserve source object; remove only unwinding metadata in the derived ELF.
    "$OBJCOPY" --remove-section=.ARM.exidx --remove-section=.rel.ARM.exidx "$src" "$clean"
  else
    cp "$src" "$clean"
  fi
  "$READELF" -r "$clean" | grep -q 'There are no relocations in this file' || {
    echo "FAIL residual relocations $arch" >&2;exit 1;
  }
  "$OBJCOPY" -O binary -j .text "$clean" "build/fb_usb_desc_${arch}.bin"
  test -s "build/fb_usb_desc_${arch}.bin" || exit 1
done
echo 'PASS: pure low-level descriptor decoder with no unresolved ARM symbols (no physical USB claims)'
