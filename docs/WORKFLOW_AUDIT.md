# Workflow audit and unique composition

Audit date: 2026-11-15  
Baseline: `8b06254ebf22b902adbc618013a684d56132e7d4`

| workflow | unique role | entry | evidence boundary | audit result |
|---|---|---|---|---|
| fastboot-orchestrator.yml | sole automatic router and summary | PR, main push, manual | composition only | ADDED |
| fastboot-evidence.yml | S15 falsifiability contract | reusable, manual | schemas and negative controls | PASS_SCOPED previously; rerun required |
| fastboot-isa.yml | source/ISA gates | reusable, manual | host, native ARM64, emulated ARMv7 | PASS_SCOPED previously; rerun required |
| miassistant-android-source.yml | Android source contract | reusable, manual | static Python + Java 8 classifier | PASS_SCOPED previously; rerun required |
| fastboot-closure-s16.yml | independent rebuild + authority gate | reusable, manual | 15 hashes then physical/provider authority | expected NO_GO until authority evidence exists |

## Findings and corrections

- Duplicate automatic triggers existed across specialist workflows. They are now reusable components; the orchestrator is the sole automatic entrypoint.
- Existing checkout/upload/download action pins used Node 20 generations. Pins now resolve to official Node 24 action manifests: checkout v5, upload-artifact v7.0.2 and download-artifact v8.0.2.
- Immutable SHA pinning, least-privilege permissions, exact-head checkout and fail-closed S16 semantics are preserved.
- No workflow result after this change is asserted here; successor runs are required.
- YAML semantic parsing by an external linter remains TOKEN_VAZIO until CI executes. The repository test performs bounded structural checks without external packages.

## Rollback

Revert the successor commit as a reviewed change. Do not delete earlier runs or receipts. A rollback restores the former triggers and Node 20 pins, so it is a recovery option, not the desired steady state.
