#!/bin/sh
set -eu
mkdir -p build
CC=${CC:-cc}
CLANG=${CLANG:-clang}
OBJCOPY=${OBJCOPY:-llvm-objcopy}
NM=${NM:-nm}
READELF=${READELF:-readelf}
"$CC" -std=c11 -O2 -Wall -Wextra -Werror -Wshadow -Wconversion \
    -Iinclude src/fb_omega.c tests/test_asm_session.c asm/x86_64/fb_session.S \
    -o build/test_asm_session
./build/test_asm_session
"$CLANG" -target armv7a-none-eabi -marm -march=armv7-a \
    -c asm/armv7/fb_session.S -o build/fb_session_armv7.o
"$CLANG" -target aarch64-none-elf \
    -c asm/aarch64/fb_session.S -o build/fb_session_aarch64.o
for arch in armv7 aarch64; do
    obj="build/fb_session_${arch}.o"
    test -z "$("$NM" -u "$obj")" || { echo "FAIL undef $arch";exit 1; }
    "$READELF" -r "$obj" | grep -q 'There are no relocations' || { echo "FAIL relocations $arch";exit 1; }
    "$OBJCOPY" -O binary -j .text "$obj" "build/fb_session_${arch}.bin"
    test -s "build/fb_session_${arch}.bin" || { echo "FAIL empty binary $arch"; exit 1; }
done
if "$CLANG" -target armv7a-none-eabi -c asm/aarch64/fb_session.S -o build/invalid_session_cross_isa.o >/dev/null 2>&1; then
    echo 'FAIL wrong ISA accepted';exit 1
fi
wc -c build/fb_session_armv7.bin build/fb_session_aarch64.bin
sha256sum build/fb_session_armv7.bin build/fb_session_aarch64.bin
echo 'PASS freestanding ISA machine-state units (ARM compilation, x86 execution)'
