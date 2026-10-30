"""Compiled functions under an original assembly file's size-0 NOTYPE labels.

The original keeps no size for an assembly file's entry points (targsupp.s,
__exception.s, gc_idct.asm). A unit may place a compiled function at such a
label; the build compares its bytes like any other code.
"""
import copy
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from code_map import build_code_map
from project_build import check_linked_labels, progress, validate_units
from setup import CONFIG, ORIG


def symbol(name, address, size=0, kind=0, binding=1, section=1):
    return dict(name=name, address=address, size=size, type=kind, binding=binding, section=section)


class FakeElf:
    def __init__(self, symbols, size=0x40):
        self.records = symbols
        self.sections = [dict(index=0, name="", address=0, size=0, flags=0),
                         dict(index=1, name=".text", address=0x1000, size=size, flags=6)]

    def symbols(self):
        return iter(self.records)


# targsupp.s: four global size-0 labels, 8 bytes apart, before InitMetroTRK.
TARGSUPP = {
    "id": "targsupp_test", "source": "src/targsupp.c", "category": "restored_library",
    "compiler_family": "GC", "compiler_version": "1.3.2", "compiler_flags": [], "include_dirs": [],
    "strip_unused": False, "sections": [{"name": ".text", "address": "0x80137060", "size": 32, "type": 1}],
    "functions": [], "externals": {}, "undefined_in_discarded_code": [], "discarded_functions": [],
    "upstream": {}, "local_externals": [], "discarded_local_functions": [], "sda_externals": [],
    "original_file": "targsupp.s", "original_file_index": 11505,
    "label_functions": [{"name": "TRKAccessFile", "address": "0x80137060", "size": 32, "binding": 1}],
}


class OriginalLabels(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if not (ORIG / "Moh2RelGC.elf").exists():
            raise unittest.SkipTest("Original executable is not imported")
        from audit import load_target
        _, cls.original = load_target()

    def unit(self):
        return copy.deepcopy(TARGSUPP)

    def test_label_function_is_valid(self):
        validate_units(self.original, [self.unit()])

    def test_label_function_is_credited(self):
        project = json.loads((CONFIG / "project.json").read_text())
        result = progress(self.original, [self.unit()], project)
        self.assertEqual(result["matching_code_bytes"], 32)
        self.assertEqual(result["matching_functions"], 1)

    def test_code_without_a_label_function_is_uncovered(self):
        unit = self.unit()
        del unit["label_functions"]
        with self.assertRaisesRegex(ValueError, "Code coverage excludes part of a function"):
            validate_units(self.original, [unit])

    def test_label_function_must_cover_the_section(self):
        unit = self.unit()
        unit["label_functions"][0]["size"] = 24
        with self.assertRaisesRegex(ValueError, "Code coverage excludes part of a function"):
            validate_units(self.original, [unit])

    def test_label_function_needs_an_original_label(self):
        unit = self.unit()
        unit["label_functions"][0]["name"] = "TRKOpenFile"  # A label, but not at this address.
        with self.assertRaisesRegex(ValueError, "lacks one original size-0 label"):
            validate_units(self.original, [unit])

    def test_label_function_keeps_the_label_binding(self):
        unit = self.unit()
        unit["label_functions"][0]["binding"] = 2
        with self.assertRaisesRegex(ValueError, "lacks one original size-0 label"):
            validate_units(self.original, [unit])

    def test_sized_function_cannot_be_a_label_function(self):
        unit = self.unit()
        unit["sections"][0].update(address="0x80137080", size=148)
        unit["label_functions"] = [{"name": "InitMetroTRK", "address": "0x80137080", "size": 148, "binding": 1}]
        with self.assertRaisesRegex(ValueError, "Original function coverage differs|lacks one original size-0 label"):
            validate_units(self.original, [unit])

    def test_label_function_cannot_swallow_a_sized_function(self):
        unit = self.unit()
        unit["sections"][0]["size"] = 32 + 148
        unit["functions"] = [{"name": "InitMetroTRK", "address": "0x80137080", "size": 148, "binding": 1}]
        unit["label_functions"][0]["size"] = 32 + 148
        with self.assertRaisesRegex(ValueError, "overlaps a sized original symbol"):
            validate_units(self.original, [unit])

    def test_code_map_counts_label_functions_as_accepted(self):
        accepted = [(0x80137060, 0x80137080)]
        code_map = build_code_map(self.original, accepted)
        ranges = [r for u in code_map["units"] for r in u["ranges"]]
        self.assertFalse([r for r in ranges if int(r["address"], 16) < 0x80137080
                          and 0x80137060 < int(r["address"], 16) + r["size"]])


class LinkedLabels(unittest.TestCase):
    unit = {"label_functions": [{"name": "first", "address": "0x1000", "size": 16, "binding": 1}]}
    original = FakeElf([symbol("first", 0x1000), symbol("second", 0x1008), symbol("after", 0x1010)])

    def test_entry_labels_match(self):
        linked = FakeElf([symbol("first", 0x1000, 16, kind=2), symbol("second", 0x1008), symbol("after", 0x1010)])
        check_linked_labels(self.original, linked, self.unit)

    def test_missing_entry_label_fails(self):
        linked = FakeElf([symbol("first", 0x1000, 16, kind=2)])
        with self.assertRaisesRegex(ValueError, "lacks an original label"):
            check_linked_labels(self.original, linked, self.unit)

    def test_moved_entry_label_fails(self):
        linked = FakeElf([symbol("first", 0x1000, 16, kind=2), symbol("second", 0x100c)])
        with self.assertRaisesRegex(ValueError, "lacks an original label"):
            check_linked_labels(self.original, linked, self.unit)

    def test_invented_global_label_fails(self):
        linked = FakeElf([symbol("first", 0x1000, 16, kind=2), symbol("second", 0x1008), symbol("extra", 0x1004)])
        with self.assertRaisesRegex(ValueError, "defines a label the original lacks"):
            check_linked_labels(self.original, linked, self.unit)

    def test_local_branch_label_is_allowed(self):
        # gc_idct's second pass is a local entry point the original names nowhere.
        linked = FakeElf([symbol("first", 0x1000, 16, kind=2), symbol("second", 0x1008),
                          symbol("PS_IDCT_col", 0x1004, binding=0)])
        check_linked_labels(self.original, linked, self.unit)


class PortLabels(unittest.TestCase):
    def test_text_layout_keeps_a_function_at_a_label(self):
        from port_unit import text_layout

        class Original:
            def globals(self, name):
                return [s for s in [symbol("TRKAccessFile", 0x80137060)] if s["name"] == name]

        class Obj:
            sections = [dict(index=0, name="", size=0), dict(index=1, name=".text", size=32)]

            def symbols(self):
                return iter([symbol("TRKAccessFile", 0, 32, kind=2), symbol("TRKOpenFile", 8)])

        layout, discarded = text_layout(Obj(), Original(), {})
        self.assertEqual([(f["name"], packed) for f, packed in layout[1][0]], [("TRKAccessFile", 0)])
        self.assertEqual(layout[1][1], 32)
        self.assertFalse(discarded)


if __name__ == "__main__":
    unittest.main()
