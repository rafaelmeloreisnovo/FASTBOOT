# Receipt — Mi Assistant Android host S1

μID: MU-FASTBOOT-MIASSISTANT-MOTOE7-REDMI12-20261115-S1  
timestamp: 2026-11-15 America/Sao_Paulo  
producer: rafaelmeloreisnovo/FASTBOOT  
base: main@ad08aec52684b298ceb31c5b25e62bb8af2359b6  
review: PR #9, draft/HOLD  
pre-receipt HEAD: d4cde33ce41f15177048d9655512f24945086dd3

## Baseline

PR #8 added a read-only Android facade. Its first implementation coupled a pure
classifier to an unused USB transport argument and inferred HyperOS from
android-version. Neither inference was supported by a Fastboot evidence
contract.

## Delta

- Moto E7 Power remains the intended ARM32 Android USB host.
- Redmi 12 model 23053RN02A / expected product fire remains the target.
- USB transport and identity classification are separated.
- HyperOS evidence is typed as
  TOKEN_VAZIO_FASTBOOT_ANDROID_VERSION_DOES_NOT_PROVE_HYPEROS_BUILD.
- Java 8 behavioral regressions cover match, mismatch, missing values and null.
- Static gate rejects destructive Fastboot wire-command literals.

## Evidence

- Prior PR #9 source-contract run 43758406297: SUCCESS.
- Prior PR #9 ISA run 43758406048: SUCCESS.
- Prior S16 run 43758406070:
  - bytewise build a: SUCCESS;
  - bytewise build b: SUCCESS;
  - 15-artifact manifest comparison: SUCCESS;
  - enforced provider/physical authority NO-GO: expected FAILURE.
- Current successor HEAD CI: NOT_OBSERVED at receipt creation.

## Typed gaps

- TOKEN_VAZIO_CI_EXACT_SUCCESSOR_HEAD
- TOKEN_VAZIO_ANDROID_SDK_NDK_BUILD
- TOKEN_VAZIO_APK_DEX_ELF_JNI
- TOKEN_VAZIO_MOTO_E7_USB_HOST_PERMISSION
- TOKEN_VAZIO_REDMI12_USB_DESCRIPTORS
- TOKEN_VAZIO_PHYSICAL_GETVAR
- TOKEN_VAZIO_INDEPENDENT_REVIEW

Canonical target identity is ASCII: REDMI12 / 23053RN02A / fire.

claim_allowed=false  
merge_allowed=false

## Falsifiers

Reject the change if product fire no longer matches, another product matches,
missing values are fabricated, HyperOS is inferred from android-version, any
destructive command literal appears, Java compilation fails, or provider
NO-GO ceases to block unsupported promotion.

## Rollback

Keep PR #9 unmerged, or supersede it with a reviewed correction. Do not rewrite
or delete PR #8, prior runs, or this receipt.
