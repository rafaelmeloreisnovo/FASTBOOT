#!/bin/sh
# Host-side negative gate: an x86 runner must never masquerade as ARM evidence.
set -eu
case "$(uname -m)" in
  arm*|aarch64)
    printf '%s\n' 'NOT_APPLICABLE: ARM host must use run_native_arm.sh with exact external SHA'
    exit 0 ;;
esac
rc=0
out=$(sh tests/run_native_arm.sh 2>&1) || rc=$?
[ "$rc" -eq 77 ] || { echo "FAIL: non-ARM gate rc=$rc (expected 77)" >&2; exit 1; }
case "$out" in
  *'NOT_RUN: host kernel is not ARM'*) : ;;
  *) echo 'FAIL: non-ARM run missing explicit NOT_RUN' >&2; exit 1 ;;
esac
case "$out" in
  *'PASS_SCOPED'*) echo 'FAIL: false ARM execution receipt detected' >&2; exit 1 ;;
esac
printf '%s\n' 'PASS: non-ARM gate emits NOT_RUN with exit 77, no false native receipt'
