#!/bin/sh
# Build-time tools exist outside the module; the object is freestanding.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT"
CLANG=${CLANG:-clang}
OBJCOPY=${OBJCOPY:-llvm-objcopy}
SHA256=${SHA256:-sha256sum}
OUT=${OUT:-build/l0-isolated}
mkdir -p "$OUT"
: > "$OUT/SHA256SUMS"
for arch in armv7 aarch64 x86_64; do
  case "$arch" in
    armv7) target=armv7a-none-eabi; link=armelf_linux_eabi ;;
    aarch64) target=aarch64-none-elf; link=aarch64elf ;;
    x86_64) target=x86_64-unknown-none-elf; link=elf_x86_64 ;;
  esac
  srcdir=asm/$arch
  for part in getvar classify session; do
    src="$srcdir/fb_$part.S"
    [ -r "$src" ] || { echo "FAIL: missing source $src" >&2; exit 65; }
    if grep -Ein '^[[:space:]]*(#[[:space:]]*include|\.include|\.incbin|\.extern|%include)\b' "$src"; then
      echo "FAIL: external source inclusion $src" >&2; exit 66
    fi
    extra=''
    [ "$arch" = armv7 ] && extra='-marm -march=armv7-a'
    "$CLANG" -target "$target" $extra -c "$src" -o "$OUT/${arch}_${part}.o"
    sh tests/verify_l0_elf.sh "$arch" "$OUT/${arch}_${part}.o"
    "$OBJCOPY" -O binary -j .text "$OUT/${arch}_${part}.o" "$OUT/${arch}_${part}.bin"
    [ -s "$OUT/${arch}_${part}.bin" ] || exit 67
    "$SHA256" "$OUT/${arch}_${part}.bin" >> "$OUT/SHA256SUMS"
  done
  # Pure concatenation of independently verified zero-relocation text. No
  # linker, synthetic alignment trap, startup, dynamic loader or CRT.
  bundle="$OUT/${arch}_combined.bin"
  layout="$OUT/${arch}_offsets.csv"
  : > "$bundle"
  printf 'name,offset_bytes,length_bytes\n' > "$layout"
  offset=0
  for part in getvar classify session; do
    part_bin="$OUT/${arch}_${part}.bin"
    size=$(wc -c < "$part_bin" | tr -d ' ')
    if [ "$arch" != x86_64 ] && [ $((offset % 4)) -ne 0 ]; then
      echo "FAIL ARM alignment: offset=$offset" >&2; exit 68
    fi
    cat "$part_bin" >> "$bundle"
    printf '%s,%s,%s\n' "$part" "$offset" "$size" >> "$layout"
    offset=$((offset + size))
  done
  [ "$(wc -c < "$bundle" | tr -d ' ')" -eq "$offset" ] || exit 69
  "$SHA256" "$bundle" >> "$OUT/SHA256SUMS"
done
printf '%s\n' 'PASS_SCOPED: 9 pure ASM ELF objects + 3 raw .text bundles verified; no runtime IO'
