# Host profile — Motorola Moto E7 Power

Status: USER_OBSERVED | DEVICE_NOT_PROBED_BY_SOFTWARE | claim_allowed=false

## Identity and versions

| Field | User-observed value | Architectural use |
|---|---|---|
| Host | Motorola Moto E7 Power | Android USB host candidate |
| Android | 10 | Compatibility target; no API newer than Android 10 |
| System build | QOLS30.288-52-23 | Exact host provenance |
| Android security patch | 2023-02-05 | Security-risk boundary |
| Google Play system update | 2021-05-01 | Stale component boundary; no trust promotion |
| Baseband | MOLY.LR12A.R3.MP.V127.6.P27 | Provenance only; does not prove USB behavior |
| Kernel | 4.9.190+ | Kernel provenance; USB Host/OTG remains unmeasured |
| Kernel build | #1 Wed Mar 8 11:51:22 CST 2023 | Exact observed build string |

Source class: user transcription from the physical host settings. This is
DOCUMENTED_USER_OBSERVED, not an ADB readback, kernel probe or independent
measurement.

## Security profile

The assistant must remain offline-first and read-only:

- no INTERNET permission;
- no dynamic code download or remote command channel;
- no credential, serial or descriptor upload;
- USB permission requested through Android UsbManager only;
- commands restricted to bounded getvar;
- no flash, erase, unlock, boot or reboot;
- fail closed when product, interface or endpoints are ambiguous;
- no storage of serial number unless the user explicitly exports a receipt.

## Compatibility gates

1. Build Java/DEX compatible with Android 10.
2. Produce native armeabi-v7a artifact only when JNI exists.
3. Verify package manifest omits INTERNET and dangerous write paths.
4. On the Moto E7 Power, observe UsbManager permission and device enumeration.
5. Capture Redmi 12 interface class FF/subclass 42/protocol 03 and bulk endpoints.
6. Send only getvar:product; accept target only if the parsed value is fire.
7. Preserve timeout, denial, disconnect and malformed reply as typed failures.

## Typed unknowns

- TOKEN_VAZIO_MOTO_E7_OTG_ELECTRICAL_SUPPORT
- TOKEN_VAZIO_USB_PERMISSION_RUNTIME
- TOKEN_VAZIO_KERNEL_USB_HOST_DRIVER_PATH
- TOKEN_VAZIO_CABLE_POWER_AND_ROLE
- TOKEN_VAZIO_REDMI12_FASTBOOT_DESCRIPTOR
- TOKEN_VAZIO_PHYSICAL_GETVAR_PRODUCT
- TOKEN_VAZIO_APK_INSTALL_AND_LAUNCH

Canonical target identity is ASCII: REDMI12 / 23053RN02A / fire.

## Rollback

Keep PR #9 unmerged if any gate fails. Correct observations by an append-only
successor; do not erase the original report.
