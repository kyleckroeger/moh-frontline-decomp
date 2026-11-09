"""The committed port manifest matches the manifests, snapshot and drafts."""
import subprocess
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class PortManifestCurrent(unittest.TestCase):
    def test_port_manifest_is_current(self):
        r = subprocess.run([sys.executable, str(ROOT / "tools/port_manifest.py"), "--check"],
                           capture_output=True, text=True)
        self.assertEqual(r.returncode, 0, r.stderr)


if __name__ == "__main__":
    unittest.main()
