"""Checks of the local source build; skipped until reconstruct.py has run.

Rising Sun's unit mutation tests (data ownership, small-data anchors, vtables,
discarded code) were specific to its accepted units. Add equivalent mutations
here alongside the first accepted Frontline source units.
"""
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from audit import load_target
from formats import Elf32
from project_build import function_name, progress, validate_object, validate_units
from setup import CONFIG


class SourceVerification(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.build = ROOT / 'build/reconstruction'
        if not (cls.build / 'report.json').exists():
            raise unittest.SkipTest('Run reconstruct.py first')
        cls.original_path, cls.original = load_target()
        cls.project = json.loads((CONFIG / 'project.json').read_text())
        cls.units = [json.loads((CONFIG / n).read_text()) for n in cls.project['units']]

    def test_complete_image_identical(self):
        report = json.loads((self.build / 'report.json').read_text())
        self.assertEqual(report['status'], 'identical')
        self.assertEqual(report['progress'], progress(self.original, self.units, self.project))

    def test_denominator_is_every_executable_byte(self):
        result = progress(self.original, self.units, self.project)
        self.assertEqual(result['total_executable_code_bytes'], 1414572)

    def test_smaller_denominator_rejected(self):
        project = json.loads(json.dumps(self.project))
        project['progress']['executable_sections']['.text'] -= 4
        project['progress']['code_bytes'] -= 4
        with self.assertRaisesRegex(ValueError, 'denominator'):
            progress(self.original, self.units, project)


class CodeSectionCoverage(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if not (ROOT / 'build/reconstruction/report.json').exists():
            raise unittest.SkipTest('Run reconstruct.py first')
        cls.original_path, cls.original = load_target()
        cls.unit = json.loads((CONFIG / '__start.json').read_text())

    def test_init_unit_is_valid(self):
        validate_units(self.original, [self.unit])

    def test_init_coverage_must_include_whole_functions(self):
        unit = json.loads(json.dumps(self.unit))
        next(s for s in unit['sections'] if s['name'] == '.init')['size'] -= 4
        with self.assertRaisesRegex(ValueError, 'Code coverage excludes part of a function'):
            validate_units(self.original, [unit])

    def test_init_coverage_cannot_skip_a_function(self):
        unit = json.loads(json.dumps(self.unit))
        unit['functions'] = unit['functions'][1:]
        with self.assertRaisesRegex(ValueError, 'Original function coverage differs'):
            validate_units(self.original, [unit])


class LeadingFiller(unittest.TestCase):
    """player_begincrouch's .bss follows 24 bytes of earlier player.cpp data."""
    @classmethod
    def setUpClass(cls):
        cls.input = ROOT / 'build/reconstruction/units/player_begincrouch/input.o'
        if not cls.input.exists():
            raise unittest.SkipTest('Run reconstruct.py first')
        cls.original_path, cls.original = load_target()
        cls.unit = json.loads((CONFIG / 'player_begincrouch.json').read_text())

    def mutated(self, filler, offset=None):
        unit = json.loads(json.dumps(self.unit))
        unit['leading_filler'] = filler
        bss = next(s for s in unit['sections'] if s['name'] == '.bss')
        bss['linked_offset'] = filler if offset is None else offset
        bss['linked_size'] += filler - self.unit['leading_filler']
        return unit

    def test_unit_is_valid(self):
        validate_units(self.original, [self.unit])
        validate_object(Elf32(self.input.read_bytes()), self.unit)

    def test_filler_must_be_the_trimmed_lead(self):
        with self.assertRaisesRegex(ValueError, 'Leading filler does not stand in'):
            validate_units(self.original, [self.mutated(24, 16)])

    def test_filler_must_exceed_small_data(self):
        with self.assertRaisesRegex(ValueError, 'Leading filler does not stand in'):
            validate_units(self.original, [self.mutated(8)])

    def test_filler_must_be_canonical(self):
        with self.assertRaisesRegex(ValueError, 'Leading filler is not the canonical'):
            validate_object(Elf32(self.input.read_bytes()), self.mutated(40))


if __name__ == '__main__':
    unittest.main()


class NumberedLocalFunctions(unittest.TestCase):
    def test_numbered_local_function_compares_by_identifier(self):
        self.assertEqual(function_name('__arraydtor$497', 0), function_name('__arraydtor$11', 0))

    def test_global_and_plain_names_are_unchanged(self):
        self.assertEqual(function_name('__arraydtor$497', 1), '__arraydtor$497')
        self.assertEqual(function_name('__sinit_dmesh_cpp', 0), '__sinit_dmesh_cpp')
        self.assertNotEqual(function_name('__arraydtor$497', 0), function_name('__arraydtor2$497', 0))

