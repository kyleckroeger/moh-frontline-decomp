"""Linker-stripped stub classes (Screen.cpp); skipped until reconstruct.py has run.

Screen.cpp keeps minimal stubs of CScreenFrameMode and CScreenFieldModePAL,
whose code, virtual tables and type records the original linker removed but
whose type names survive. The manifest lists them in `linker_stripped`; the
build discards them object by object.
"""
import copy
import json
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from audit import load_target
from data_strip import linker_stripped, strip_objects
from formats import Elf32
from port_unit import kept_span
from project_build import validate_object, validate_units
from setup import CONFIG


class FakeObject:
    """Just enough of an Elf32 for kept_span: one extab section, no relocations."""
    def __init__(self, objects):
        self.sections = [{"index": 0, "name": "", "type": 0, "flags": 0, "size": 0},
                         {"index": 1, "name": "extab", "type": 1, "flags": 2,
                          "size": sum(size for _, _, size in objects)}]
        self._symbols = [{"name": name, "section": 1, "address": address, "size": size, "type": 1, "binding": 0}
                         for name, address, size in objects]

    def symbols(self):
        return iter(self._symbols)


class StubKeptSpan(unittest.TestCase):
    """A linker-stripped function's exception record from the middle of a
    section, moved to the end by data_strip, must not be kept because a
    numbered record of the same size follows the unit in the original
    (CCompartment::HandleExplosion and the unreferenced fast_vec<int> dtor)."""
    obj = FakeObject([("@136", 0, 24), ("@148", 24, 8), ("@142", 32, 8)])
    numbered = {"@1": [{"name": "@1", "address": 0x100, "size": 24}],
                "@2": [{"name": "@2", "address": 0x118, "size": 8}],
                "@3": [{"name": "@3", "address": 0x120, "size": 8}]}

    def test_numbered_records_are_kept_by_address(self):
        self.assertEqual(kept_span(self.obj, 1, [], None, self.numbered, 0x100), (0, 40, []))

    def test_moved_stub_record_is_not_kept(self):
        self.assertEqual(kept_span(self.obj, 1, [], None, self.numbered, 0x100, {"@142"}), (0, 32, []))


class FakeRelocatable:
    """An Elf32 stand-in with .text, extab and extabindex and their relocations.

    `symbols` are (name, section, address, size, type); `relocs` are
    (section, offset, symbol name) with no addend.
    """
    NAMES = ("", ".text", "extab", "extabindex")

    def __init__(self, symbols, relocs):
        self._symbols = [{"name": n, "section": sec, "address": a, "size": size, "type": t, "binding": 0}
                         for n, sec, a, size, t in symbols]
        index = {s["name"]: i for i, s in enumerate(self._symbols)}
        self.sections = [{"index": i, "name": n, "type": 1 if i else 0, "flags": 2 if i else 0,
                          "size": max([s["address"] + s["size"] for s in self._symbols if s["section"] == i] or [0])}
                         for i, n in enumerate(self.NAMES)]
        self._contents = {}
        for i in range(1, len(self.NAMES)):
            rows = [(off, index[name]) for sec, off, name in relocs if sec == i]
            if rows:
                self._contents[len(self.sections)] = b"".join(
                    struct.pack(">IIi", off, (sym << 8) | 1, 0) for off, sym in rows)
                self.sections.append({"index": len(self.sections), "name": ".rela", "type": 4, "flags": 0,
                                      "info": i, "size": 12 * len(rows)})

    def symbols(self):
        return iter(self._symbols)

    def contents(self, section):
        return self._contents[section["index"]]


class DroppedCodeKeptSpan(unittest.TestCase):
    """Exception records of code the unit drops (a discarded static, a file
    duplicate) are never kept, although the original has a numbered record
    of the same size at the placed address (every 8- or 12-byte record finds
    one): sound.cpp, bsbifunc.cpp and objcreate.cpp fragments, scene_cmppairs."""
    # Kept functions A and C, dropped B between them. B's exception table
    # names the kept destructor C (it points at kept code too).
    obj = FakeRelocatable(
        [("A", 1, 0, 16, 2), ("B", 1, 16, 16, 2), ("C", 1, 32, 16, 2),
         ("@10", 2, 0, 8, 1), ("@11", 2, 8, 16, 1), ("@12", 2, 24, 8, 1),
         ("@20", 3, 0, 12, 1), ("@21", 3, 12, 12, 1), ("@22", 3, 24, 12, 1)],
        [(3, 0, "A"), (3, 8, "@10"), (3, 12, "B"), (3, 20, "@11"), (3, 24, "C"), (3, 32, "@12"),
         (2, 12, "C")])
    kept = [({"name": "A", "section": 1, "address": 0, "size": 16}, 0),
            ({"name": "C", "section": 1, "address": 32, "size": 16}, 16)]
    # The original numbers a record of every compiled size at every address.
    numbered = {f"@{a:x}.{n}": [{"name": f"@{a:x}.{n}", "address": a, "size": n}]
                for a in range(0x100, 0x140, 4) for n in (8, 12, 16)}

    def test_dropped_function_records_are_stripped(self):
        self.assertEqual(kept_span(self.obj, 2, self.kept, None, self.numbered, 0x100), (0, 32, ["@11"]))
        self.assertEqual(kept_span(self.obj, 3, self.kept, None, self.numbered, 0x100), (0, 36, ["@21"]))

    def test_trailing_dropped_records_are_trimmed(self):
        kept = self.kept[:1]
        self.assertEqual(kept_span(self.obj, 2, kept, None, self.numbered, 0x100), (0, 8, []))
        self.assertEqual(kept_span(self.obj, 3, kept, None, self.numbered, 0x100), (0, 12, []))


class LinkerStripped(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        path = ROOT / 'build/reconstruction/units/Screen/compiled.o'
        if not path.exists():
            raise unittest.SkipTest('Run reconstruct.py first')
        cls.compiled = path.read_bytes()
        cls.unit = json.loads((CONFIG / 'Screen.json').read_text())
        if 'linker_stripped' not in cls.unit:
            raise unittest.SkipTest('Screen is not the whole-file unit')
        cls.original_path, cls.original = load_target()

    def mutated(self, kind, names):
        unit = copy.deepcopy(self.unit)
        unit['linker_stripped'][kind] = names
        return unit

    def test_unit_is_valid(self):
        validate_units(self.original, [self.unit])
        validate_object(Elf32(strip_objects(self.compiled, self.unit)), self.unit)

    def test_stubs_bring_their_records(self):
        functions, objects = linker_stripped(Elf32(self.compiled), self.unit)
        self.assertEqual(len(functions), 4)
        obj = Elf32(self.compiled)
        sections = {s['index']: s['name'] for s in obj.sections}
        kinds = sorted(sections[o['section']] for o in objects)
        # Two virtual tables and base lists (.data), two type records (.sdata),
        # and the two destructors' exception records.
        self.assertEqual(kinds, ['.data'] * 4 + ['.sdata'] * 2 + ['extab'] * 2 + ['extabindex'] * 2)
        # The type names themselves stay: they survive in the original.
        self.assertNotIn('.rodata', kinds)

    def test_stubs_move_after_the_kept_slices(self):
        obj = Elf32(strip_objects(self.compiled, self.unit))
        sizes = {s['name']: s['size'] for s in self.unit['sections']}
        sections = {s['index']: s['name'] for s in obj.sections}
        names = self.unit['linker_stripped']['objects']
        for symbol in obj.symbols():
            if symbol['name'] in names:
                self.assertGreaterEqual(symbol['address'], sizes[sections[symbol['section']]], symbol['name'])

    def test_missing_listed_object_fails(self):
        unit = self.mutated('objects', self.unit['linker_stripped']['objects'] + ['__vt__13CScreenMissing'])
        with self.assertRaisesRegex(ValueError, 'not one compiled data object: __vt__13CScreenMissing'):
            strip_objects(self.compiled, unit)

    def test_function_listed_as_object_fails(self):
        unit = self.mutated('objects', ['__dt__16CScreenFrameModeFv'])
        with self.assertRaisesRegex(ValueError, 'not one compiled data object'):
            strip_objects(self.compiled, unit)

    def test_stub_inside_the_kept_data_fails(self):
        unit = copy.deepcopy(self.unit)
        sdata = next(s for s in unit['sections'] if s['name'] == '.sdata')
        sdata['size'] = sdata['linked_size']
        with self.assertRaisesRegex(ValueError, 'inside the kept data'):
            strip_objects(self.compiled, unit)

    def test_unlisted_stub_is_not_discarded(self):
        unit = self.mutated('objects', [n for n in self.unit['linker_stripped']['objects']
                                        if n != '__RTTI__19CScreenFieldModePAL'])
        with self.assertRaises(ValueError):
            obj = Elf32(strip_objects(self.compiled, unit))
            validate_object(obj, unit)

    def test_definition_present_in_the_target_fails(self):
        unit = self.mutated('objects', self.unit['linker_stripped']['objects'] + ['__vt__7CScreen'])
        with self.assertRaisesRegex(ValueError, 'Cannot strip a definition present in the target'):
            validate_units(self.original, [unit])

    def test_listed_function_must_be_discarded(self):
        unit = copy.deepcopy(self.unit)
        unit['discarded_functions'].remove('__dt__19CScreenFieldModePALFv')
        with self.assertRaisesRegex(ValueError, 'Linker-stripped definitions need'):
            validate_units(self.original, [unit])


if __name__ == '__main__':
    unittest.main()
