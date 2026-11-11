"""The committed port manifest matches the manifests, snapshot and drafts.

Skipped without the original ELF (CI has none), like the other local-only tests.
"""
import subprocess
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


@unittest.skipUnless((ROOT / "orig/GMFE69/Moh2RelGC.elf").is_file(),
                     "needs the original ELF (local only)")
class PortManifestCurrent(unittest.TestCase):
    def test_port_manifest_is_current(self):
        r = subprocess.run([sys.executable, str(ROOT / "tools/port_manifest.py"), "--check"],
                           capture_output=True, text=True)
        self.assertEqual(r.returncode, 0, r.stderr)


if __name__ == "__main__":
    unittest.main()
