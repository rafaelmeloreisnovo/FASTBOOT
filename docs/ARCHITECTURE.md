# FASTBOOT Ω L0 — USB host boundary (2026-10-23)

SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM. TOKEN_VAZIO != 0.

## Ownership
- src/ + include/: producer-authoritative portable freestanding parser/state machine. No USB I/O, libc, heap, syscalls, JNI, external dependencies, mutable globals or firmware-write commands.
- host/linux_usbfs.c: Linux notebook OS-dependent host connector, requiring kernel usbfs and USB device permissions. It is **not** freestanding.
- host/android/AndroidFastbootUsbTransport.java: Android USB Host adapter source verifies permission, selects Fastboot interface and bulk endpoints, and allows bounded getvar only. Java syntax compiled against synthetic Android SDK stubs locally; real Android SDK, JNI/NDK wiring, and physical OTG remain unverified.
- Notebook as USB target for Android host: TOKEN_VAZIO_NOTEBOOK_USB_GADGET absent a notebook supporting USB device/gadget mode. Do not confuse with notebook host → phone target.
- Mi Assistant / Mi Recovery: TOKEN_VAZIO_OEM_PROTOCOL; separate from Fastboot, never infer from mere USB connectivity.

## Protocol
Host-initiated USB bulk endpoints. Commands strictly getvar:<key>. Initial L0 uses a 64-byte command ceiling for older Fastboot 0.4 interoperability. Parses bounded replies up to 256 bytes: OKAY, FAIL, INFO, TEXT, DATA. DATA is rejected without entering the data phase. INFO/TEXT bounded at 32 messages. Unknown, truncated, oversized and malformed DATA are fail-closed; host operations carry a 2500 ms timeout. No automatic write retries.

AOSP protocol: https://android.googlesource.com/platform/system/core/+/master/fastboot/README.md
AOSP legacy 0.4: https://android.googlesource.com/platform/system/core/+/refs/tags/android-cts-7.0_r33/fastboot/fastboot_protocol.txt

## Linux notebook → phone
Determine and verify physical USB descriptors and Fastboot interface first; use USB class FF, subclass 42, protocol 03 only after confirming target's descriptor. Never rely only on vendor/product ID.

Example **placeholder** (interface and endpoints must be replaced with actual observed values):

~~~sh
./build/fb-linux /dev/bus/usb/BBB/DDD 0 0x01 0x81 getvar product
~~~

Linux host adapter does not enumerate descriptors by itself; the user must verify USB interface, endpoints, permissions, and device path independently. No root-free guarantee.

## Android phone → phone
OS UsbManager permission → Fastboot interface discovery and two bulk endpoints → Android host adapter → native JNI/NDK bridge → **same L0**. Android OS transport source exists but JNI bridge, APK and device tests are not implemented/observed.

## Gates
G0 freestanding object: -ffreestanding -fno-builtin -nostdlib; nm -u must be empty.
G1 synthetic protocol tests including failures and bounds.
G2 Linux usbfs adapter compilation.
G3 USB hardware end-to-end and Android JNI/USB host: TOKEN_VAZIO_PHYSICAL.

claim_allowed=false | flash_allowed=false | unlock_allowed=false | promotion=false.
