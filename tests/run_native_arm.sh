#!/bin/sh
# S7: opt-in native ARM ABI execution gate. This does not talk to USB.
# libc/compiler/git/sha256sum are host test utilities only, never linked to ASM objects.
set -eu

# Make the absence of ARM execution explicit; x86 must never report PASS.
case "$(uname -m)" in
  arm*|aarch64) : ;;
  *) printf '%s\n' 'NOT_RUN: host kernel is not ARM; zero physical-execution claims'; exit 77 ;;
esac

: "${CC:=cc}"
: "${EXPECTED_HEAD:?Set EXPECTED_HEAD to the independently verified exact GitHub commit SHA}"
case "$EXPECTED_HEAD" in
  *[!0-9a-f]*|'') echo 'FAIL: EXPECTED_HEAD must be lowercase hex' >&2; exit 65 ;;
esac
[ "${#EXPECTED_HEAD}" -eq 40 ] || { echo 'FAIL: EXPECTED_HEAD requires 40 hex bytes' >&2; exit 65; }
# A local untracked test script must not inherit the checked-out SHA receipt.
# It will only become eligible after this script is included in Git HEAD.
git cat-file -e HEAD:tests/run_native_arm.sh 2>/dev/null || {
  echo 'FAIL: native test script is not committed in the exact source HEAD' >&2; exit 69;
}
git cat-file -e HEAD:tests/check_native_arm_negative.sh 2>/dev/null || {
  echo 'FAIL: source-negative control is not committed in the exact source HEAD' >&2; exit 69;
}
actual_head=$(git rev-parse --verify HEAD)
[ "$actual_head" = "$EXPECTED_HEAD" ] || {
  echo "FAIL: source drift actual=$actual_head expected=$EXPECTED_HEAD" >&2; exit 66;
}
# Tracked test and ISA source must match Git HEAD. Previous commits preserved.
git diff --quiet HEAD -- include/fb_omega.h src/fb_omega.c tests/test_asm_getvar.c \
  tests/test_asm_classify.c tests/test_asm_session.c asm/armv7 asm/aarch64 tests/run_native_arm.sh || {
  echo 'FAIL: locally modified source (receipt must name a clean head)' >&2; exit 67;
}
git diff --cached --quiet HEAD -- include/fb_omega.h src/fb_omega.c tests/test_asm_getvar.c \
  tests/test_asm_classify.c tests/test_asm_session.c asm/armv7 asm/aarch64 tests/run_native_arm.sh || {
  echo 'FAIL: staged modifications change the test source' >&2; exit 67;
}

# Query target compiler predefined ISA, not Android version, kernel bitness or ABI guess.
macros=$(printf '\n' | "$CC" -dM -E -x c -)
case "$macros" in
  *'__aarch64__'*) isa=aarch64; abi=LP64 ;;
  *'__arm__'*) isa=armv7; abi=ILP32 ;;
  *) echo 'NOT_RUN: compiler target is not ARM (cross toolchain not accepted)' >&2; exit 77 ;;
esac

root="build/native-arm/$EXPECTED_HEAD/$isa"
mkdir -p "$root"
cat > "$root/abi_guard.c" <<'GUARD'
#include <stdint.h>
#include <stddef.h>
#if !defined(__arm__) && !defined(__aarch64__)
#error ARM execution gate requires an ARM target
#endif
#if defined(__aarch64__)
_Static_assert(sizeof(void *)==8, "aarch64 LP64 required");
#else
_Static_assert(sizeof(void *)==4, "ARMv7 ILP32 required");
#endif
_Static_assert(sizeof(uint32_t)==4, "U32 contract violated");
GUARD
"$CC" -std=c11 -Werror -c "$root/abi_guard.c" -o "$root/abi_guard.o"

# Compile and execute three independent pre-existing differential suites.
# Never link any Linux USB transport, Android SDK or device modification API.
for name in getvar classify session; do
  "$CC" -std=c11 -O2 -Wall -Wextra -Werror -Iinclude \
    src/fb_omega.c "tests/test_asm_$name.c" "asm/$isa/fb_$name.S" \
    -o "$root/test_$name"
  "$root/test_$name" || {
    echo "FAIL: ARM ABI test_$name, stopping without a PASS receipt" >&2; exit 68;
  }
done

# Receipt is append-only, source SHA-bound, not a hardware attestation.
{
  printf 'UTC=%s\n' "$(date -u +%Y-%m-%dT%H:%M:%SZ)"
  printf 'SOURCE_HEAD=%s\nTARGET_ISA=%s\nABI=%s\n' "$EXPECTED_HEAD" "$isa" "$abi"
  printf 'EXECUTION=NATIVE_ARM_USERSPACE_THREE_SUITES_PASS\n'
  printf 'USB=NOT_RUN\nSILICON_ATTESTATION=NOT_OBSERVED\n'
  printf 'CLAIM_ALLOWED=false\nPROMOTION=false\n'
  sha256sum "$root/test_getvar" "$root/test_classify" "$root/test_session"
  printf '%s\n' '---'
} | tee -a "$root/receipts-append-only.txt"
echo "PASS_SCOPED: native ARM userland; receipt=$root/receipts-append-only.txt"
