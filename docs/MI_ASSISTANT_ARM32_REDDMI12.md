# Mi Assistant USB — Moto E7 Power host → Redmi 12 alvo

Status: SOURCE_IMPLEMENTED | PHYSICAL_NOT_RUN | claim_allowed=false

## Topologia real

- **Host/computador:** Motorola Moto E7 Power, Android ARM32, ABI de referência armeabi-v7a.
- **Transporte:** USB OTG/USB Host.
- **Alvo:** Redmi 12, modelo 23053RN02A, codename Fastboot esperado fire.
- **Sistema observado no alvo:** Android 15, HyperOS 2.0.209.0.VMXMIXM.
- **Compatibilidade futura:** Android ARM64 com ABI arm64-v8a.

O Moto E7 Power executa o Mi Assistant; o Redmi 12 não executa o aplicativo. O Redmi 12 somente responde ao transporte USB em Fastboot.

## Escopo

- Reutiliza AndroidFastbootUsbTransport.
- Lê somente product, serialno, slot-count e android-version do Redmi 12.
- Confere provisoriamente o codename fire.
- Observa se android-version contém HyperOS.
- Não executa unlock, erase, flash, boot, reboot, patch ou protocolo proprietário Xiaomi.

## ABI

A execução prioritária é no Moto E7 Power em armeabi-v7a. A camada Java é comum; o artefato nativo/JNI pode também ser produzido para arm64-v8a em outro host Android compatível. O ABI do host não altera o modelo nem o firmware do Redmi 12.

## Gates

| Gate | Resultado |
|---|---|
| Moto E7 Power ARM32 como host | SOURCE_ONLY |
| Host ARM64 compatível | SOURCE_ONLY |
| APK/DEX/ELF/JNI | TOKEN_VAZIO — toolchain não presente |
| Cabo OTG + Redmi 12 físico | TOKEN_VAZIO — não executado |
| HyperOS | OBSERVATION_ONLY |
| Protocolo Mi Assistant OEM | TOKEN_VAZIO_OEM_PROTOCOL |

Não declarar PASS global sem build armeabi-v7a, descritores USB, receipt do Moto E7 Power, receipt do Redmi 12 e readback.
