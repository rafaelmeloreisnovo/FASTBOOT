#!/bin/sh
# Build/test-only toolchain. None of these utilities is linked to freestanding L0.
set -eu
: "${EXPECTED_HEAD:?set exact SHA}"
case "$EXPECTED_HEAD" in *[!0-9a-f]*|'') echo 'FAIL_INVALID_SHA' >&2; exit 65;; esac
[ "${#EXPECTED_HEAD}" -eq 40 ] || exit 65
[ "$(git rev-parse HEAD)" = "$EXPECTED_HEAD" ] || { echo 'FAIL_SOURCE_DRIFT' >&2; exit 66; }
: "${S16_SLOT:?slot a or b}"
case "$S16_SLOT" in a|b) ;; *) exit 65;; esac
for t in clang nm readelf sha256sum; do command -v "$t" >/dev/null || exit 77; done
selected=''
for v in llvm-objcopy llvm-objcopy-21 llvm-objcopy-20 llvm-objcopy-19 llvm-objcopy-18 llvm-objcopy-17 llvm-objcopy-16 llvm-objcopy-15 llvm-objcopy-14; do
  if command -v "$v" >/dev/null 2>&1; then selected=$(command -v "$v"); break; fi
done
[ -n "$selected" ] || { echo 'NOT_RUN_NO_LLVM_OBJCOPY' >&2; exit 77; }
# The original build script calls llvm-objdump only from its static ELF gate.
selected_dis=''
for v in llvm-objdump llvm-objdump-21 llvm-objdump-20 llvm-objdump-19 llvm-objdump-18 llvm-objdump-17 llvm-objdump-16 llvm-objdump-15 llvm-objdump-14; do
  if command -v "$v" >/dev/null 2>&1; then selected_dis=$(command -v "$v"); break; fi
done
[ -n "$selected_dis" ] || { echo 'NOT_RUN_NO_LLVM_OBJDUMP' >&2; exit 77; }
OUT='build/s16-independent'
mkdir -p "$OUT" 'build/s16-evidence'
OBJCOPY="$selected" OBJDUMP="$selected_dis" OUT="$OUT" sh tests/build_l0_isolated.sh
(
  cd "$OUT"
  LC_ALL=C find . -type f \( -name '*.bin' -o -name '*_offsets.csv' \) -print | LC_ALL=C sort | while IFS= read -r f; do
    sha256sum "$f"
  done
) > build/s16-evidence/manifest.sha256
[ "$(wc -l < build/s16-evidence/manifest.sha256 | tr -d ' ')" -eq 15 ] || {
  echo 'FAIL_UNEXPECTED_ARTIFACT_COUNT' >&2; exit 68;
}
{
  printf 'SOURCE_SHA=%s\nSLOT=%s\nRUNNER_ARCH=%s\n' "$EXPECTED_HEAD" "$S16_SLOT" "$(uname -m)"
  clang --version | head -1
  "$selected" --version | head -1
} > "build/s16-evidence/environment-$S16_SLOT.txt"
cp build/s16-evidence/manifest.sha256 "build/s16-evidence/manifest-$S16_SLOT.sha256"
sha256sum "build/s16-evidence/manifest-$S16_SLOT.sha256"
echo 'PASS_SCOPED=BUILD_ONLY_15_ARTIFACT_HASHES; NO_INDEPENDENT_TRUST_ROOT_OR_PHYSICAL_HARDWARE_CLAIM'
