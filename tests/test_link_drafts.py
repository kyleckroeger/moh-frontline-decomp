"""build_elf in research/tools/link_drafts.py, on a small synthetic ELF."""
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "research/tools"))
from link_drafts import (ARENA_LO_HI, ARENA_LO_LO, ARENA_STACK_HI, ARENA_STACK_LO, build_elf,
                         choose_address, references_in)

PHOFF, PHSIZE, SHSIZE = 0x34, 32, 40
CODE_ADDRESS = 0x80110000
CODE_SIZE = 0x600
FIRST_OFFSET = 0x60  # Room for one header only, as tight as the game's ELF.
SECTION_DATA = b"section!"


def synthetic_elf():
    """One load segment, then section data and a two-entry section table."""
    code = bytes(range(256)) * (CODE_SIZE // 256)
    section_at = FIRST_OFFSET + CODE_SIZE
    shoff = section_at + len(SECTION_DATA)
    data = bytearray(FIRST_OFFSET)
    data[:7] = b"\x7fELF\x01\x02\x01"
    struct.pack_into(">II", data, 28, PHOFF, shoff)
    struct.pack_into(">HHHH", data, 42, PHSIZE, 1, SHSIZE, 2)
    struct.pack_into(">8I", data, PHOFF, 1, FIRST_OFFSET, CODE_ADDRESS, 0, CODE_SIZE, CODE_SIZE, 5, 32)
    data += code + SECTION_DATA
    data += struct.pack(">10I", *[0] * 10)
    data += struct.pack(">10I", 0, 1, 0, 0, section_at, len(SECTION_DATA), 0, 0, 1, 0)
    return bytes(data), code


class BuildElf(unittest.TestCase):
    ARENA = 0x80363ce0
    BLOB = b"\xaa" * 64

    def setUp(self):
        self.original, self.code = synthetic_elf()
        self.patch = CODE_ADDRESS + 0x20
        self.data, self.end = build_elf(self.original, [(self.patch, self.ARENA)], self.ARENA, self.BLOB)
        self.count = struct.unpack_from(">H", self.data, 44)[0]
        self.headers = [struct.unpack_from(">8I", self.data, PHOFF + i * PHSIZE) for i in range(self.count)]

    def word(self, address):
        h = self.headers[0]
        return struct.unpack_from(">I", self.data, h[1] + address - h[2])[0]

    def test_program_headers_stay_at_the_start(self):
        self.assertEqual(struct.unpack_from(">I", self.data, 28)[0], PHOFF)
        self.assertEqual(self.count, 2)
        table_end = PHOFF + self.count * PHSIZE
        self.assertTrue(all(h[1] >= table_end for h in self.headers))

    def test_new_segment_holds_the_blob(self):
        h = self.headers[1]
        self.assertEqual((h[2], h[4], h[5]), (self.ARENA, len(self.BLOB), len(self.BLOB)))
        self.assertEqual(self.data[h[1]:h[1] + h[4]], self.BLOB)

    def test_function_start_branches_to_its_draft(self):
        self.assertEqual(self.word(self.patch), 0x48000000 | ((self.ARENA - self.patch) & 0x03fffffc))

    def test_both_arena_assignments_move_past_the_segment(self):
        self.assertEqual(self.end, (self.ARENA + len(self.BLOB) + 31) & ~31)
        for hi, lo in ((ARENA_LO_HI, ARENA_LO_LO), (ARENA_STACK_HI, ARENA_STACK_LO)):
            high = self.word(hi) & 0xffff
            low = struct.unpack(">h", struct.pack(">H", self.word(lo) & 0xffff))[0]
            self.assertEqual(((high << 16) + low) & 0xffffffff, self.end)

    def test_other_code_is_unchanged(self):
        changed = {self.patch, ARENA_LO_HI, ARENA_LO_LO, ARENA_STACK_HI, ARENA_STACK_LO}
        h = self.headers[0]
        for off in range(0, CODE_SIZE, 4):
            if CODE_ADDRESS + off not in changed:
                self.assertEqual(self.data[h[1] + off:h[1] + off + 4], self.code[off:off + 4])

    def test_section_table_still_points_at_its_data(self):
        shoff = struct.unpack_from(">I", self.data, 32)[0]
        at = struct.unpack_from(">I", self.data, shoff + SHSIZE + 16)[0]
        self.assertEqual(self.data[at:at + len(SECTION_DATA)], SECTION_DATA)


class DuplicateNames(unittest.TestCase):
    def test_direct_reference_wins(self):
        self.assertEqual(choose_address({0x100, 0x900}, [0x900, 0x5000]), 0x900)

    def test_nearest_to_a_section_anchor(self):
        self.assertEqual(choose_address({0x100, 0x900}, [0x880]), 0x900)

    def test_no_hints_is_deterministic(self):
        self.assertEqual(choose_address({0x900, 0x100}, []), 0x100)

    def test_references_stay_within_the_function(self):
        refs = [(0x10, 1), (0x20, 2), (0x24, 3), (0x30, 4)]
        self.assertEqual(references_in(refs, 0x20, 0x10), [2, 3])


if __name__ == "__main__":
    unittest.main()
