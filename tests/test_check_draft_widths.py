"""check_draft.py reads an access's size from its mnemonic, so a byte field written as a word is caught."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "research/tools"))
from check_draft import width  # noqa: E402


class Widths(unittest.TestCase):
    def test_sizes(self):
        for mn, size in (("lbz", 1), ("stbx", 1), ("lhz", 2), ("lha", 2), ("sthu", 2), ("lwz", 4),
                         ("stwu", 4), ("lfs", 4), ("stfsx", 4), ("lfd", 8), ("stfdu", 8), ("psq_l", "psq")):
            self.assertEqual(width(mn), size, mn)

    def test_not_memory(self):
        for mn in ("addi", "lmw", "stmw", "li", "lis"):
            self.assertIsNone(width(mn), mn)

    def test_byte_flag_written_as_word(self):
        # CCompartment::Draw: the original sets a one-byte flag with stb; a draft with
        # "int m_pending" used stw, same offset, and wrote three bytes past the flag.
        self.assertNotEqual(width("stb"), width("stw"))


if __name__ == "__main__":
    unittest.main()
