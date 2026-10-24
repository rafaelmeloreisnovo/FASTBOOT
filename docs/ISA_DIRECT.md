# FASTBOOT Ω — ISA direta: montagem ARMv7/AArch64/x86-64

Contrato: SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM; TOKEN_VAZIO != 0.

## Núcleo

Os arquivos asm/armv7/fb_getvar.S, asm/aarch64/fb_getvar.S e
asm/x86_64/fb_getvar.S codificam a operação estritamente read-only
`getvar:<chave>` em instruções da respectiva ISA: valida a chave ASCII
de 1 a 57 bytes [a-zA-Z0-9_.-], grava os sete bytes 'getvar:' seguidos
pela chave e devolve 7+n; erro devolve 0 sem modificar a saída.
Não escreve byte NUL. Buffers de entrada/saída devem ser distintos,
entrada legível e saída de ao menos 64 bytes graváveis.

Nenhum desses módulos possui #include, runtime C, heap, ABI JNI, SDK,
Gradle, ART, DEX, R8 ou NDK, nem chamada ao sistema. Produzir o ELF usa
toolchain, que é ferramenta de construção e não dependência do objeto.
Entradas e retornos são registradores, não classes/objetos.

| ABI | entrada | comprimento | saída | retorno | temporários |
|---|---|---|---|---|---|
| ARMv7 AAPCS | r0 | r1 | r2 | r0 | r3, r12 |
| AArch64 AAPCS64 | x0 | x1 | x2 | x0 | x3, w4 |
| x86-64 SysV | rdi | rsi | rdx | rax | rcx, r8 |

Chamar a rotina requer ABI, memória executável e buffers válidos.
O nome do símbolo é uma etiqueta do ELF; não aloca objeto em execução.

## Gates

Execute separadamente `sh tests/run_asm.sh`. No host x86-64:
14.848 vetores de equivalência byte a byte com o predecessor C11
(n=1..58 e primeiro byte 0..255), mais quatro casos inválidos;
ASan/UBSan PASS local (harness de testes contém libc, fora do módulo).
Clang montou ARMv7 ELF32 e AArch64 ELF64, `nm -u` vazio e
`readelf -r` sem relocações; seções `.text` extraídas em `.bin`.
O teste negativo rejeita código AArch64 ao selecionar alvo ARMv7.

Fonte ARMv7: 224 bytes de `.text`, SHA256
805b6f270c1db9b5d4d97c6d2716ac5a852bed810e59f109ea85e5b58ee9b5c6.
Fonte AArch64: 212 bytes de `.text`, SHA256
30e800af866cb77fb5db2792f3dbb1752223ec67598263ba84ce9647a07b3fe2.

Os bytes `.bin` são *rotinas*, não arquivos executáveis autônomos e
não são firmware. Os testes ARM foram apenas montagem: TOKEN_VAZIO_ARM_RUNTIME.
Não houve teste físico USB/OTG nem validação de um controlador USB bare-metal.
Fabricantes/SoCs têm controladores, PHY, relógios, DMA, MMIO, permissões e
inicialização específicas: nenhum assembler ou compilador os inventa.

`PUSH`, `PULL` e `INT` não são operações universalmente presentes em
todas as ISAs. A arquitetura muda as instruções e as codificações.
Branchs e iterações ainda existem quando comprimento é variável;
apenas não são escritas como constructs de linguagem de alto nível.

## Governança

Este é um sucessor experimental em paralelo ao L0 C e ao guard anterior.
Não modifica transporte USB, parser, adaptadores ou main.
Sem flash, erase, unlock, reboot, escrita no firmware, failover automático
ou alegação de execução ARM. claim_allowed=false; promotion=false;
rollback=predecessor exato preservado.
