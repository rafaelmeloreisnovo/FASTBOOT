# FASTBOOT Ω S15 — Catálogo falsificável, tokens tipados e cadeia de custódia

**Classificação:** estudo de engenharia sujeito a revisão humana, NÃO paper aprovado ou certificação.
**Base S14 integrada:** `e38183978b61e7e6414c7a13fcf9162d740de2af` (PR #5, merged at 2026-10-24T21:10:47Z).
**Papel:** validador Python 3 host-only para registros acadêmicos/auditoria, fora do L0 freestanding.
**Fronteira:** o núcleo C/ASM e a interface Linux USB não foram modificados pelo S15.

## Tipagem — não confundir ausência com sucesso

Cada entrada tem `gate`, `hypothesis`, `falsifier`, `experiment`,
`acceptance`, `rollback`, `status` e `references`.
Status reconhecidos: NOT_RUN, NOT_OBSERVED, UNVERIFIED, BLOCKED,
HYPOTHESIS, OBSERVED_GAP, PASS_SCOPED. A hipótese não pode herdar
PASS_SCOPED; evidências históricas pertencem aos respectivos SHAs.

O campo `unresolved_token_zero=false` é invariável: um dado faltante
é um estado epistêmico tipado, não o número 0. Valores `claim_allowed=true`
e `promotion=true` são rejeitados enquanto qualquer gate está aberto.

## Critério acadêmico mínimo de falsificabilidade

1. Hipótese clara e refutável (pode haver uma observação que a contradiga).
2. Procedimento, condição de refutação e controle negativo explícitos.
3. Métrica ou observação que possa ser repetida por examinador distinto.
4. Registro de origem, SHA, ambiente e recibo que não herde conclusões.
5. Alternativas, vieses e limitação de validade externa declarados.
6. Consulta a trabalhos anteriores; familiaridade com pesquisas ≠ novidade.
7. Separação entre sucesso instrumental local e efetividade com hardware.
8. Revisão por pessoas competentes, direitos autorais, privacidade e consentimento.

## Oito perguntas testáveis (G14 → S15)

| Gate | Experimento refutador | Estado S15 |
|---|---|---|
| G14-01 | Tentar merge não aprovado após proteção efetiva | OBSERVED_GAP |
| G14-02 | Dois rebuilds independentes e comparação de hashes | NOT_RUN |
| G14-03 | Teste físico com periférico USB imitador autorizado | UNVERIFIED |
| G14-04 | Adulteração do recibo assinado para verificar rejeição | NOT_RUN |
| G14-05 | Injetar recibo órfão, SHA alterado e escopo falso | UNVERIFIED para completude externa |
| G14-06 | Comparar SHA de merge com runs e artefatos no SHA exato | NOT_OBSERVED |
| G14-07 | Confrontar antecedente científico e replicação independente | UNVERIFIED |
| G14-08 | Revisão humana de acessibilidade, dados, menores e cultura local | NOT_RUN |

## Receipts, testes e rollback

`tests/verify_s15.py` verifica os arquivos originais S14 + S15
sem acesso a rede, usando apenas bibliotecas padrão do Python para
auditoria host. No controle negativo local, 16 mutações inseguras foram
rejeitadas: auto-promoção, falsa evidência física, scope de produção,
SHA malformado, token zerado, claim órfão, herança indevida de SHA
e falsificador ausente. Esses 16 PASS medem o AUDITOR; não
validam a hipótese científica ou os aparelhos.

Um recibo `PASS_SCOPED` só pode sustentar claim do mesmo SHA e
escopo; não se converte em PASS para o merge nem em atestação
criptográfica. A execução CI S13 de origem é
https://github.com/rafaelmeloreisnovo/FASTBOOT/actions/runs/40129145151
(com SHA da PR S13), e não equivale a CI do S14/S15 ou execução USB física.

A PR #5 foi observada MERGED com main `e38183978b61e7e6414c7a13fcf9162d740de2af`;
`main.protected=false` e `rulesets=[]` foram observados, não sanados.
Quem realizou/autorizou o merge não foi determinado.
Este documento não autoriza manipular a branch principal ou reverter merges.

**Rollback:** restaurar a branch sucessora à referência de predecessor
por nova PR autorizada, preservando histórico e arquivos; nunca force/delete
automaticamente. **Promover só mediante revisão humana e evidência no SHA
exato.** SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM,
TOKEN_VAZIO != 0, claim_allowed=false, promotion=false.

## Literatura externa — anterioridade, não validação

- [Reproducible Builds and Independent Verifier](https://www.alphaxiv.org/abs/2505.21642)
- [USBIPS](https://www.alphaxiv.org/abs/2409.12850)
- [Kettle: Attested Builds](https://www.alphaxiv.org/abs/2605.08363)
- [From Agent Traces to Trust](https://www.alphaxiv.org/abs/2606.04990)

Nenhum artigo foi reproduzido experimentalmente aqui. Citar não concede
certificação, inventividade acadêmica ou transferência de autoria.
