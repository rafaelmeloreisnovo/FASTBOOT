#!/bin/sh
# Toolchain-only gates: no runtime dependencies for the Assembly objects.
set -eu
mkdir -p build
CC=${CC:-cc}
CLANG=${CLANG:-clang}
OBJCOPY=${OBJCOPY:-llvm-objcopy}
NM=${NM:-nm}
READELF=${READELF:-readelf}
"$CC" -std=c11 -O2 -Wall -Wextra -Werror -Iinclude \
  src/fb_omega.c tests/test_asm_getvar.c asm/x86_64/fb_getvar.S \
  -o build/test_asm_getvar
./build/test_asm_getvar
"$CLANG" -target armv7a-none-eabi -marm -march=armv7-a \
  -c asm/armv7/fb_getvar.S -o build/fb_asm_armv7.o
"$CLANG" -target aarch64-none-elf \
  -c asm/aarch64/fb_getvar.S -o build/fb_asm_aarch64.o
for arch in armv7 aarch64; do
  obj=build/fb_asm_${arch}.o
  test -z "$("$NM" -u "$obj")" || { echo "FAIL undefined symbols $arch" >&2; exit 1; }
  "$READELF" -r "$obj" | grep -q 'There are no relocations' || {
    echo "FAIL non-independent text $arch" >&2; exit 1;
  }
  "$OBJCOPY" -O binary -j .text "$obj" "build/fb_asm_${arch}.bin"
  test -s "build/fb_asm_${arch}.bin" || { echo "FAIL empty binary $arch" >&2; exit 1; }
done
if "$CLANG" -target armv7a-none-eabi -c \
  asm/aarch64/fb_getvar.S -o build/invalid_cross_isa.o >/dev/null 2>&1; then
  echo 'FAIL incorrect ISA accepted' >&2
  exit 1
fi
echo "PASS ISA boundaries; no undefined externals or relocations"
wc -c build/fb_asm_armv7.bin build/fb_asm_aarch64.bin
sha256sum build/fb_asm_armv7.bin build/fb_asm_aarch64.bin
