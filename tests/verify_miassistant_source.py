#!/usr/bin/env python3
"""Fail-closed source contract for the Android USB assistant."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
ASSISTANT = ROOT / "host/android/MiAssistantArm32.java"
TRANSPORT = ROOT / "host/android/AndroidFastbootUsbTransport.java"

def fail(message):
    print("FAIL:", message, file=sys.stderr)
    raise SystemExit(1)

def require(text, token):
    if token not in text:
        fail("missing required contract token: " + token)

assistant = ASSISTANT.read_text(encoding="utf-8")
transport = TRANSPORT.read_text(encoding="utf-8")

for token in (
    'TARGET_MODEL = "23053RN02A"',
    'EXPECTED_PRODUCT = "fire"',
    'HOST_ABI_ARM32 = "armeabi-v7a"',
    'HOST_ABI_ARM64 = "arm64-v8a"',
    'TOKEN_VAZIO_FASTBOOT_ANDROID_VERSION_DOES_NOT_PROVE_HYPEROS_BUILD',
    'inspect(GetvarReader reader)',
    'reader.get("product")',
    'reader.get("serialno")',
    'reader.get("slot-count")',
    'reader.get("android-version")',
):
    require(assistant, token)

if "inspect(AndroidFastbootUsbTransport" in assistant:
    fail("pure classifier is incorrectly coupled to USB transport")
if "contains(\"hyperos\")" in assistant.lower():
    fail("android-version was promoted to HyperOS evidence")

require(transport, "writeGetvar")
require(transport, "only getvar allowed")
require(transport, "USB_ENDPOINT_XFER_BULK")

for forbidden in ("flash:", "erase:", "oem unlock", "flashing ", "boot:", "reboot"):
    if re.search(r'["\']' + re.escape(forbidden), assistant + transport, re.I):
        fail("destructive wire-command literal present: " + forbidden)

print("PASS_SCOPED: source contract is read-only and HyperOS uncertainty is typed")
