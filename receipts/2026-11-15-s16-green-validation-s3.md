# Receipt — green validation with separate release authority

μID: FASTBOOT-S16-GREEN-VALIDATION-S3  
timestamp: 2026-11-15 America/Sao_Paulo  
parent: 2cdad9701e70ed4e223b8987c58fd42b90d5f9d0  
delta: split technical validation from release authorization  
validation mode: missing physical/provider authority is recorded as NO_GO with claim_allowed=false but does not fail correct compilation/tests  
release mode: enforce_release=true preserves fail-closed red status when authority is missing  
falsifiers: unexpected verifier rc; missing receipt; failed manifests; failed mutation tests; release enforcement returning green without authority  
rollback: reviewed revert preserving all prior runs and receipts  
F_next: observe successor run and keep PR draft until physical USB evidence exists
