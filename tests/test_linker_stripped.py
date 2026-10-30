"""Linker-stripped stub classes (Screen.cpp); skipped until reconstruct.py has run.

Screen.cpp keeps minimal stubs of CScreenFrameMode and CScreenFieldModePAL,
whose code, virtual tables and type records the original linker removed but
whose type names survive. The manifest lists them in `linker_stripped`; the
build discards them object by object.
"""
import copy
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from audit import load_target
from data_strip import linker_stripped, strip_objects
from formats import Elf32
from project_build import validate_object, validate_units
from setup import CONFIG


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
