"""Mutations of the listed extab padding patches; object tests skip until reconstruct.py has run."""
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from extab_patch import PATCHES, apply_patches, extab_record, record_actions
from formats import Elf32

# A large-table record: one code range whose only action is a new[]
# cleanup (DELETEPOINTERCOND, end of list) with padding 066c at +0x16.
NEW_ARRAY = bytes.fromhex("200800000000003c000a0010000000008bc0001c001d066c00000000")
# A catch(...) block (CATCHBLOCK_32, padding at +0x1a) and an active catch block.
CATCH_ALL = bytes.fromhex("481a000000000088000000180000009800040028000000009000ff7f"
                          "000000000000008c000000088d000008")


class ExtabActions(unittest.TestCase):
    def test_actions_are_walked_from_the_code_ranges(self):
        self.assertEqual(record_actions(NEW_ARRAY), {0x10: 11})
        self.assertEqual(record_actions(CATCH_ALL), {0x18: 16, 0x28: 13})

    def test_an_action_past_the_record_fails(self):
        with self.assertRaises(ValueError):
            record_actions(NEW_ARRAY[:0x18])


class ListedPatches(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.patches = json.loads(PATCHES.read_text())["patches"]
        cls.build = ROOT / 'build/reconstruction/units'
        if not cls.patches or not all((cls.build / p["unit"] / 'compiled.o').exists() for p in cls.patches):
            raise unittest.SkipTest('Run reconstruct.py first')

    def compiled(self, patch):
        return (self.build / patch["unit"] / 'compiled.o').read_bytes()

    def test_only_the_listed_bytes_change(self):
        for patch in self.patches:
            with self.subTest(function=patch["function"]):
                data = self.compiled(patch)
                patched = apply_patches(data, [patch])
                extab, start, _ = extab_record(Elf32(data), patch["function"])
                at = extab["offset"] + start + int(patch["offset"], 16)
                self.assertEqual(patched[at:at + 2], bytes.fromhex(patch["original"]))
                self.assertEqual(patched[:at] + patched[at + 2:], data[:at] + data[at + 2:])

    def test_other_compiler_output_fails(self):
        for patch in self.patches:
            with self.subTest(function=patch["function"]):
                changed = dict(patch, compiled=patch["original"])
                with self.assertRaises(ValueError):
                    apply_patches(self.compiled(patch), [changed])

    def test_a_field_outside_a_cleanup_padding_fails(self):
        for patch in self.patches:
            for delta in (-4, -2, 2, 4):
                with self.subTest(function=patch["function"], delta=delta):
                    moved = dict(patch, offset=hex(int(patch["offset"], 16) + delta))
                    with self.assertRaises(ValueError):
                        apply_patches(self.compiled(patch), [moved])


if __name__ == "__main__":
    unittest.main()
