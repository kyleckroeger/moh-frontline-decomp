"""The generated parts of the docs match the committed snapshot and manifests."""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from sync_docs import expected


class DocsCurrent(unittest.TestCase):
    def test_generated_docs_are_current(self):
        for name, text in expected(ROOT).items():
            self.assertEqual((ROOT / name).read_text(), text, f"{name} is stale: run python3 tools/sync_docs.py")


if __name__ == "__main__":
    unittest.main()
