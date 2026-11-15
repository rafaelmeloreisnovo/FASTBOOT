# Mi Assistant ARM32 USB — Redmi 12

Status: SOURCE_IMPLEMENTED | PHYSICAL_NOT_RUN | claim_allowed=false

This branch adds a read-only Android facade for a Redmi 12 Fastboot host running on an ARM32 Android device (armeabi-v7a at APK/native build time).

## Scope

- Reuses AndroidFastbootUsbTransport.
- Reads only product, serialno, slot-count and android-version.
- Matches Redmi 12 by product codename fire.
- Reports whether android-version contains HyperOS.
- Does not unlock, erase, flash, boot, reboot, patch or invoke Xiaomi proprietary Mi Assistant protocols.

## ABI boundary

Java bytecode is architecture-neutral. The ARM32 requirement applies to the eventual native/JNI/APK artifact: native ABI armeabi-v7a, shared C/ASM L0, and no JNI/APK PASS until compiled and tested with an Android toolchain.

## Gates

| Gate | Result |
|---|---|
| Source added | IMPLEMENTED |
| Existing bounded transport reused | REVIEW_REQUIRED |
| ARM32 APK/DEX/ELF build | TOKEN_VAZIO — toolchain absent in repository |
| Redmi 12 physical USB | TOKEN_VAZIO — not run |
| HyperOS identification | OBSERVATION_ONLY |
| Mi Assistant OEM protocol | TOKEN_VAZIO_OEM_PROTOCOL |

Do not claim global PASS without Android build output, descriptor capture, device receipt and readback evidence.
