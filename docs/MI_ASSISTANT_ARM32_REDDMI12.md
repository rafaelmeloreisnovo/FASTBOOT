# Mi Assistant USB — host ARM32/ARM64, alvo Redmi 12

Status: SOURCE_IMPLEMENTED | PHYSICAL_NOT_RUN | claim_allowed=false

O Redmi 12 é o dispositivo-alvo. O Mi Assistant é executado em outro dispositivo Android com USB Host/OTG, em ARM32 (armeabi-v7a) ou ARM64 (arm64-v8a).

## Escopo

- Reutiliza AndroidFastbootUsbTransport.
- Lê somente product, serialno, slot-count e android-version do Redmi 12 em Fastboot.
- Confere provisoriamente o codename fire.
- Observa se android-version contém HyperOS.
- Não executa unlock, erase, flash, boot, reboot, patch ou protocolo proprietário Xiaomi.

## ABI

A camada Java é comum aos dois hosts. O artefato nativo/JNI deve ser produzido para armeabi-v7a e/ou arm64-v8a, conforme o host Android. O alvo Redmi 12 não define o ABI do Mi Assistant.

## Gates

| Gate | Resultado |
|---|---|
| Código-fonte | IMPLEMENTED |
| Host ARM32 | SOURCE_ONLY |
| Host ARM64 | SOURCE_ONLY |
| APK/DEX/ELF/JNI | TOKEN_VAZIO — toolchain não presente |
| Redmi 12 USB físico | TOKEN_VAZIO — não executado |
| HyperOS | OBSERVATION_ONLY |
| Protocolo Mi Assistant OEM | TOKEN_VAZIO_OEM_PROTOCOL |

Não declarar PASS global sem build, descritores USB, receipt do dispositivo e readback.
