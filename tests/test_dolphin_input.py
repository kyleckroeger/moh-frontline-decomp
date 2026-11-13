"""Dolphin test inputs: input_script.py's movie writer and link_drafts.py's coverage stubs."""
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "research/tools"))
import input_script
from link_drafts import coverage_stubs


class Movie(unittest.TestCase):
    def setUp(self):
        steps, _ = input_script.parse("at 100\npress A 10\nstick 0 72 20 R\n")
        self.steps = steps
        self.data = input_script.dtm(steps, tail=50)

    def test_header(self):
        d = self.data
        self.assertEqual(d[:4], b"DTM\x1a")
        self.assertEqual(d[4:10], b"GMFE69")
        self.assertEqual(d[11], 1)  # controller port 1
        frames, inputs = struct.unpack_from("<QQ", d, 13)
        self.assertEqual(inputs, self.steps[-1][0] + 50)
        self.assertEqual(len(d), input_script.HEADER + 8 * inputs)

    def test_records(self):
        def rec(poll):
            at = input_script.HEADER + 8 * poll
            return self.data[at:at + 8]
        self.assertEqual(rec(0), bytes([0, 0x40, 0, 0, 128, 128, 128, 128]))  # idle, connected
        self.assertEqual(rec(100), bytes([0x02, 0x40, 0, 0, 128, 128, 128, 128]))  # A
        self.assertEqual(rec(110), bytes([0, 0x40, 0, 0, 128, 128, 128, 128]))  # released
        # The stick step starts after the press's release gap: R digital + full analog.
        self.assertEqual(rec(116), bytes([0, 0x48, 0, 255, 128, 200, 128, 128]))


class Coverage(unittest.TestCase):
    def test_stub_reports_once_and_branches_on(self):
        arena, blob = 0x80400000, bytearray(b"\0" * 64)
        patches = [(0x80010000, 0x80400000), (0x80020000, 0x80400020)]
        out = coverage_stubs(patches, {0x80010000: "Foo__Fv"}, 0x80100000, arena, blob)
        stub = dict(out)[0x80010000]
        self.assertEqual(dict(out)[0x80020000], 0x80400020)  # not listed: unchanged
        words = [struct.unpack_from(">I", blob, stub - arena + 4 * i)[0] for i in range(53)]
        last = stub + 4 * 52
        self.assertEqual(words[-1] >> 26, 18)  # b
        self.assertEqual(last + ((words[-1] & 0x03fffffc) ^ 0x02000000) - 0x02000000, 0x80400000)
        self.assertIn(b"COVER Foo__Fv\n\0", bytes(blob))
        call = [i for i, w in enumerate(words) if w >> 26 == 18 and w & 1][0]
        here = stub + 4 * call
        self.assertEqual(here + ((words[call] & 0x03fffffc) ^ 0x02000000) - 0x02000000, 0x80100000)


if __name__ == "__main__":
    unittest.main()
