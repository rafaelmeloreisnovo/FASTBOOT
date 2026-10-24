#!/bin/sh
# Compile-only ARM objects, host-executed x86-64. Never confound tests with real OTG.
set -eu
mkdir -p build
CC=${CC:-cc}
CLANG=${CLANG:-clang}
NM=${NM:-nm}
READELF=${READELF:-readelf}
OBJCOPY=${OBJCOPY:-llvm-objcopy}
"$CC" -std=c11 -O2 -Wall -Wextra -Werror -Iinclude \
    src/fb_omega.c tests/test_asm_classify.c asm/x86_64/fb_classify.S \
    -o build/test_asm_classify
./build/test_asm_classify
"$CLANG" -target armv7a-none-eabi -marm -march=armv7-a -c \
    asm/armv7/fb_classify.S -o build/fb_classify_armv7.o
"$CLANG" -target aarch64-none-elf -c \
    asm/aarch64/fb_classify.S -o build/fb_classify_aarch64.o
for arch in armv7 aarch64; do
    obj="build/fb_classify_${arch}.o"
    test -z "$("$NM" -u "$obj")" || { echo "FAIL: undefined $arch"; exit 1; }
    "$READELF" -r "$obj" | grep -q 'There are no relocations' || { echo "FAIL relocations $arch"; exit 1; }
    "$OBJCOPY" -O binary -j .text "$obj" "build/fb_classify_${arch}.bin"
    test -s "build/fb_classify_${arch}.bin" || { echo "FAIL empty $arch"; exit 1; }
done
if "$CLANG" -target armv7a-none-eabi -c asm/aarch64/fb_classify.S \
    -o build/invalid_classify_cross_isa.o >/dev/null 2>&1; then
    echo 'FAIL: wrong ISA accepted' >&2; exit 1
fi
wc -c build/fb_classify_armv7.bin build/fb_classify_aarch64.bin
sha256sum build/fb_classify_armv7.bin build/fb_classify_aarch64.bin
echo 'PASS: distinct ARM ISA, no external relocations, read-only classification objects'
