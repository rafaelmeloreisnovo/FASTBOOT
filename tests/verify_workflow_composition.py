#!/usr/bin/env python3
"""Fail-closed static contract for FASTBOOT workflow composition."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
WF = ROOT / ".github" / "workflows"
ORCH = WF / "fastboot-orchestrator.yml"
SPECIALISTS = (
    "fastboot-evidence.yml",
    "fastboot-isa.yml",
    "miassistant-android-source.yml",
    "fastboot-closure-s16.yml",
)
ALLOWED = {
    "actions/checkout": "fbc6f3992d24b796d5a048ff273f7fcc4a7b6c09",
    "actions/upload-artifact": "cf430e030ddbb5b0abf93d22962f4752f3646cd9",
    "actions/download-artifact": "9000827ccba6bdab643e8b6fd33ac0654aef8333",
}
errors = []

for required in ("AGENTS.md", "AGENT.md", "docs/WORKFLOW_AUDIT.md"):
    if not (ROOT / required).is_file():
        errors.append(f"missing {required}")

pointer = (ROOT / "AGENT.md").read_text(encoding="utf-8")
if "AGENTS.md" not in pointer:
    errors.append("AGENT.md must point to AGENTS.md")

texts = {p.name: p.read_text(encoding="utf-8") for p in sorted(WF.glob("*.yml"))}
if ORCH.name not in texts:
    errors.append("missing orchestrator")

for name in SPECIALISTS:
    text = texts.get(name, "")
    if "workflow_call:" not in text:
        errors.append(f"{name}: missing workflow_call")
    if re.search(r"(?m)^  (pull_request|push):", text):
        errors.append(f"{name}: duplicate automatic entrypoint")
    call = f"uses: ./.github/workflows/{name}"
    if texts.get(ORCH.name, "").count(call) != 1:
        errors.append(f"orchestrator must call {name} exactly once")

for name, text in texts.items():
    for action, sha in re.findall(r"uses:\s+(actions/[\w-]+)@([0-9a-f]{40})", text):
        expected = ALLOWED.get(action)
        if expected != sha:
            errors.append(f"{name}: unapproved pin {action}@{sha}")
    for raw in re.findall(r"uses:\s+(actions/[\w-]+)@([^\s]+)", text):
        if not re.fullmatch(r"[0-9a-f]{40}", raw[1]):
            errors.append(f"{name}: mutable action ref {raw[0]}@{raw[1]}")

names = []
for name, text in texts.items():
    match = re.search(r"(?m)^name:\s*(.+)$", text)
    if not match:
        errors.append(f"{name}: missing name")
    else:
        names.append(match.group(1).strip())
if len(names) != len(set(names)):
    errors.append("workflow display names are not unique")

if errors:
    for error in errors:
        print(f"FAIL: {error}", file=sys.stderr)
    raise SystemExit(1)
print(f"PASS_SCOPED=WORKFLOW_COMPOSITION files={len(texts)} specialists={len(SPECIALISTS)}")
print("NOT_PROVEN=YAML_EXTERNAL_LINTER,PHYSICAL_USB,PROVIDER_AUTHORITY")
