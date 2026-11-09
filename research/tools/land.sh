#!/bin/sh
# land.sh "<commit subject>" [body]: full verification, snapshot, tests, commit (research helper)
set -e
cd "$(dirname "$0")/../.."
for f in $(git status --porcelain config/GMFE69 | awk '$1=="??"{print $2}'); do
  grep -q "\"$(basename $f)\"" config/GMFE69/project.json || { echo "UNREGISTERED $f (run research/tools/addunit.py)"; exit 1; }
done
python3 tools/reconstruct.py | tail -1 | grep -q 'Complete image identical' || { echo RECONSTRUCT FAILED; exit 1; }
out=$(python3 tools/progress_report.py --capture 2>&1) || { echo "$out" | tail -3; echo "PROGRESS CAPTURE FAILED (re-run reconstruct, then land again)"; exit 1; }
echo "$out" | tail -1
python3 tools/port_manifest.py >/dev/null || { echo PORT MANIFEST FAILED; exit 1; }
python3 tools/sync_docs.py || { echo DOC SYNC FAILED; exit 1; }
python3 -m unittest discover -s tests 2>&1 | tail -1 | grep -q OK || { echo TESTS FAILED; exit 1; }
git add -A config/GMFE69 src docs research progress include tools tests README.md AGENTS.md CONTRIBUTING.md 2>/dev/null
git commit -q -m "$1" -m "${2:-}" -m "Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
git log --oneline -1
