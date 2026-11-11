"""Data tables written in research/data/ match the original; skipped without the original ELF or compiler."""
import subprocess
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class DataTables(unittest.TestCase):
    def test_every_table_is_exact(self):
        if not (ROOT / "orig/GMFE69/Moh2RelGC.elf").exists() or not (ROOT / "build/compiler").exists():
            self.skipTest("needs the original ELF and the compiler (local only)")
        r = subprocess.run([sys.executable, str(ROOT / "research/tools/check_data.py")],
                           capture_output=True, text=True, timeout=600)
        self.assertEqual(r.returncode, 0, r.stdout[-2000:] + r.stderr[-2000:])


if __name__ == "__main__":
    unittest.main()
