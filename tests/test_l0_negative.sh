#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT"
CC=${CC:-cc}
CLANG=${CLANG:-clang}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT HUP INT TERM
# If an external call is present, undefined symbol or relocation must be caught.
printf 'extern void external(void); void unit(void){ external(); }\n' > "$TMP/external.c"
"$CC" -c "$TMP/external.c" -o "$TMP/external.o"
if sh tests/verify_l0_elf.sh x86_64 "$TMP/external.o" > "$TMP/log" 2>&1; then
  echo 'FAIL: admitted external undefined symbol' >&2; exit 1
fi
# Genuine x86 syscall instruction in isolated ET_REL must also fail.
printf '.text\n.globl syscall_injection\nsyscall_injection:\n syscall\n ret\n' > "$TMP/syscall.S"
"$CC" -c "$TMP/syscall.S" -o "$TMP/syscall.o"
if sh tests/verify_l0_elf.sh x86_64 "$TMP/syscall.o" > "$TMP/log" 2>&1; then
  echo 'FAIL: admitted syscall' >&2; exit 1
fi
# Allocated writable state violates the text-only no-global-state contract.
printf '.data\n.globl hidden_global\nhidden_global: .long 42\n.text\n.globl unit2\nunit2: ret\n' > "$TMP/global.S"
"$CC" -c "$TMP/global.S" -o "$TMP/global.o"
if sh tests/verify_l0_elf.sh x86_64 "$TMP/global.o" > "$TMP/log" 2>&1; then
  echo 'FAIL: admitted static writable state' >&2; exit 1
fi
# Wrong machine must not silently cross-promote.
"$CLANG" -target aarch64-none-elf -c asm/aarch64/fb_session.S -o "$TMP/aarch64.o"
if sh tests/verify_l0_elf.sh armv7 "$TMP/aarch64.o" > "$TMP/log" 2>&1; then
  echo 'FAIL: admitted mismatched ISA' >&2; exit 1
fi
# Non-ELF input must be refused.
if sh tests/verify_l0_elf.sh x86_64 tests/test_l0_negative.sh > "$TMP/log" 2>&1; then
  echo 'FAIL: admitted non ELF' >&2; exit 1
fi
printf '%s\n' 'PASS: rejected external symbol, syscall, writable global, wrong ISA and non-ELF'
