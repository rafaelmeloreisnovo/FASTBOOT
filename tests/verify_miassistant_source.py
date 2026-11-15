#!/usr/bin/env python3
"""Static contract for the read-only Android USB assistant source.

This test does not claim an APK, DEX, JNI, OTG, or Redmi 12 hardware run.
It fails if the source contract silently expands from getvar-only to a
destructive Fastboot command family.
"""
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
    'reader.get("product")',
    'reader.get("serialno")',
    'reader.get("slot-count")',
    'reader.get("android-version")',
):
    require(assistant, token)

require(transport, "writeGetvar")
require(transport, "only getvar allowed")
require(transport, "USB_ENDPOINT_XFER_BULK")

# Match wire-command literals only, avoiding explanatory prose/comments.
for forbidden in ("flash:", "erase:", "oem unlock", "flashing ", "boot:", "reboot"):
    if re.search(r'["\']' + re.escape(forbidden), assistant + transport, re.I):
        fail("destructive wire-command literal present: " + forbidden)

print("PASS_SCOPED: Android host source is getvar-only; no APK/JNI/OTG claim")
