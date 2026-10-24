#!/bin/sh
# Inspect an ELF relocatable object. Host-side gate: not linked into L0.
set -eu
[ "$#" -eq 2 ] || { echo 'usage: verify_l0_elf.sh armv7|aarch64|x86_64 OBJECT' >&2; exit 64; }
isa=$1
obj=$2
READELF=${READELF:-readelf}
NM=${NM:-nm}
OBJDUMP=${OBJDUMP:-llvm-objdump}
[ -r "$obj" ] || { echo "FAIL: missing ELF: $obj" >&2; exit 65; }
header=$("$READELF" -h "$obj") || { echo 'FAIL: invalid ELF' >&2; exit 65; }
printf '%s\n' "$header" | grep -Eq 'Type:[[:space:]]+REL[[:space:]]' || { echo 'FAIL: not ET_REL' >&2; exit 66; }
case "$isa" in
  armv7)
    printf '%s\n' "$header" | grep -Eq 'Class:[[:space:]]+ELF32' || exit 66
    printf '%s\n' "$header" | grep -Eq 'Machine:[[:space:]]+ARM' || exit 66 ;;
  aarch64)
    printf '%s\n' "$header" | grep -Eq 'Class:[[:space:]]+ELF64' || exit 66
    printf '%s\n' "$header" | grep -Eq 'Machine:[[:space:]]+AArch64' || exit 66 ;;
  x86_64)
    printf '%s\n' "$header" | grep -Eq 'Class:[[:space:]]+ELF64' || exit 66
    printf '%s\n' "$header" | grep -Eq 'Machine:[[:space:]]+Advanced Micro Devices X86-64' || exit 66 ;;
  *) echo 'FAIL: unknown ISA' >&2; exit 64 ;;
esac
# Strict self-contained text profile: no non-text allocatable sections.
sections=$("$READELF" -W -S "$obj") || exit 66
printf '%s\n' "$sections" | grep -Eq '[[:space:]]\.text[[:space:]]+PROGBITS[[:space:]]' || {
  echo 'FAIL: no code section' >&2; exit 66;
}
allocated_other=$(printf '%s\n' "$sections" | \
  sed -n -E 's/^[[:space:]]*\[[[:space:]]*[0-9]+\][[:space:]]*/ /p' | \
  awk '$1 != ".text" && $7 ~ /A/ { print $1 ":" $7 }')
if [ -n "$allocated_other" ]; then
  printf 'FAIL: allocated non-text section: %s\n' "$allocated_other" >&2
  exit 66
fi
undefined=$("$NM" -u "$obj") || exit 67
[ -z "$undefined" ] || { printf 'FAIL: undefined imports\n%s\n' "$undefined" >&2; exit 67; }
relocs=$("$READELF" -r "$obj") || exit 68
printf '%s\n' "$relocs" | grep -q 'There are no relocations in this file\.' || {
    printf 'FAIL: unresolved relocations\n%s\n' "$relocs" >&2; exit 68;
}
dynamic=$("$READELF" -d "$obj") || exit 69
printf '%s\n' "$dynamic" | grep -q 'There is no dynamic section in this file\.' || {
  echo 'FAIL: dynamic loader dependency' >&2; exit 69;
}
# Check disassembled mnemonics, never just raw-byte signatures.
# This is a denylist, not a formal proof of privilege-safe machine code.
disasm=$("$OBJDUMP" -d --no-show-raw-insn "$obj") || exit 70
if printf '%s\n' "$disasm" | grep -Ei '^[[:space:]]*[0-9a-f]+:[[:space:]]+(svc|swi|hvc|smc|syscall|sysenter|int3|int|blx?|blr|callq?|sys|eret|wfi|hlt|bkpt|wrmsr|rdmsr|cli|sti|lgdt|lidt|pushq?|popq?|stp|ldp)([[:space:].]|$)' >/dev/null; then
  echo 'FAIL: privileged operation or external-call instruction present' >&2; exit 71
fi
printf 'PASS_SCOPED: ELF pure object ISA=%s path=%s; no imports/relocs/dynamic, forbidden opcodes not observed\n' "$isa" "$obj"
