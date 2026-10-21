"""Mutations of per-object data stripping and binding checks; skipped until reconstruct.py has run."""
import json
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from audit import load_target
from data_strip import strip_objects
from formats import Elf32
from project_build import check_pool, validate_object, validate_units
from setup import CONFIG


def relocation_targets(elf, section_name):
    """Names of the symbols that relocations located in a section refer to."""
    symbols = list(elf.symbols())
    index = next(s["index"] for s in elf.sections if s["name"] == section_name)
    for rela in elf.sections:
        if rela["type"] == 4 and rela["info"] == index:
            for offset in range(0, rela["size"], 12):
                _, info, _ = struct.unpack_from(">IIi", elf.contents(rela), offset)
                yield symbols[info >> 8]


class DataStripping(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.build = ROOT / 'build/reconstruction'
        if not (cls.build / 'units/Pad/compiled.o').exists():
            raise unittest.SkipTest('Run reconstruct.py first')
        cls.original_path, cls.original = load_target()
        cls.unit = json.loads((CONFIG / 'Pad.json').read_text())
        cls.compiled = (cls.build / 'units/Pad/compiled.o').read_bytes()

    def mutated(self, section_name, objects):
        unit = json.loads(json.dumps(self.unit))
        next(s for s in unit['sections'] if s['name'] == section_name)['stripped_objects'] = objects
        return unit

    def test_stripped_objects_move_after_the_kept_slice(self):
        obj = Elf32(strip_objects(self.compiled, self.unit))
        section = next(s for s in obj.sections if s['name'] == '.sbss')
        manifest = next(s for s in self.unit['sections'] if s['name'] == '.sbss')
        for symbol in obj.symbols():
            if symbol['section'] == section['index'] and symbol['type'] == 1:
                stripped = symbol['name'].split('$')[0] in manifest['stripped_objects']
                self.assertEqual(symbol['address'] >= manifest['size'], stripped, symbol['name'])
        self.assertEqual(len(obj.data), len(self.compiled))

    def test_dead_data_no_longer_keeps_a_discarded_function(self):
        before = {s['name'] for s in relocation_targets(Elf32(self.compiled), '.data')}
        after = {s['name'] for s in relocation_targets(Elf32(strip_objects(self.compiled, self.unit)), '.data')}
        self.assertIn('OnReset', before)
        self.assertNotIn('OnReset', after)

    def test_stripping_a_referenced_object_fails(self):
        sbss = next(s for s in self.unit['sections'] if s['name'] == '.sbss')
        unit = self.mutated('.sbss', sbss['stripped_objects'] + ['EnabledBits'])
        with self.assertRaisesRegex(ValueError, 'Retained code or data references trimmed data'):
            validate_object(Elf32(strip_objects(self.compiled, unit)), unit)

    def test_stripping_an_object_present_in_the_target_fails(self):
        unit = self.mutated('.sbss', ['__PADSpec'])
        with self.assertRaisesRegex(ValueError, 'Cannot strip an object present in the target'):
            validate_units(self.original, [unit])

    def test_weak_binding_is_checked_before_linking(self):
        compiled = (self.build / 'units/ctype/compiled.o').read_bytes()
        unit = json.loads((CONFIG / 'ctype.json').read_text())
        self.assertEqual(unit['functions'][0]['binding'], 2)
        validate_object(Elf32(strip_objects(compiled, unit)), unit)
        unit['functions'][0]['binding'] = 1
        with self.assertRaisesRegex(ValueError, 'Compiled function binding differs'):
            validate_object(Elf32(strip_objects(compiled, unit)), unit)

    def test_input_alignment_relaxes_only_the_section_header(self):
        compiled = (self.build / 'units/smixfx/compiled.o').read_bytes()
        unit = json.loads((CONFIG / 'smixfx.json').read_text())
        before, after = Elf32(compiled), Elf32(strip_objects(compiled, unit))
        sbss = next(s for s in unit['sections'] if s['name'] == '.sbss')
        self.assertEqual((int(sbss['address'], 16) % 8, sbss['input_alignment']), (4, 4))
        for old, new in zip(before.sections, after.sections):
            expected = 4 if old['name'] == '.sbss' else old['alignment']
            self.assertEqual((old['alignment'], new['alignment']), (old['alignment'], expected), old['name'])
            self.assertEqual(dict(old, alignment=0), dict(new, alignment=0))

    def test_input_alignment_needs_a_4_mod_8_placement(self):
        unit = json.loads((CONFIG / 'smixfx.json').read_text())
        validate_units(self.original, [unit])
        mutated = json.loads(json.dumps(unit))
        next(s for s in mutated['sections'] if s['name'] == '.text')['input_alignment'] = 4
        with self.assertRaisesRegex(ValueError, 'Input alignment is not needed'):
            validate_units(self.original, [mutated])
        pad = json.loads(json.dumps(self.unit))
        aligned = next(s for s in pad['sections'] if s['name'] == '.sbss' and int(s['address'], 16) % 8 == 0)
        aligned['input_alignment'] = 4
        with self.assertRaisesRegex(ValueError, 'Input alignment is not needed'):
            validate_units(self.original, [pad])

    def test_input_alignment_only_relaxes_data(self):
        compiled = (self.build / 'units/smixfx/compiled.o').read_bytes()
        unit = json.loads((CONFIG / 'smixfx.json').read_text())
        next(s for s in unit['sections'] if s['name'] == '.text')['input_alignment'] = 4
        with self.assertRaisesRegex(ValueError, 'only relax compiler data alignment'):
            strip_objects(compiled, unit)

    def test_odd_input_alignment_needs_byte_objects(self):
        compiled = (self.build / 'units/player_death/compiled.o').read_bytes()
        unit = json.loads((CONFIG / 'player_death.json').read_text())
        section = Elf32(strip_objects(compiled, unit)).sections
        self.assertEqual([s['alignment'] for s in section if s['name'] == '.sbss'], [1])
        # The fade object itself (20 bytes) may not sit at an odd address.
        next(s for s in unit['sections'] if s['name'] == '.bss')['input_alignment'] = 1
        with self.assertRaisesRegex(ValueError, 'natural alignment'):
            strip_objects(compiled, unit)

    def test_file_duplicate_references_go_to_the_original_copy(self):
        compiled = (self.build / 'units/UISUtils_parsehints/compiled.o').read_bytes()
        unit = json.loads((CONFIG / 'UISUtils_parsehints.json').read_text())
        name = unit['file_duplicates'][0]
        after = Elf32(strip_objects(compiled, unit))
        self.assertEqual([s['binding'] for s in after.symbols() if s['name'] == name + '$duplicate'], [0])
        self.assertEqual([s['section'] for s in after.symbols() if s['name'] == name], [0])

    def weak_duplicate_unit(self):
        compiled = (self.build / 'units/staticobjfactory_lookup/compiled.o').read_bytes()
        return compiled, json.loads((CONFIG / 'staticobjfactory_lookup.json').read_text())

    def test_weak_duplicate_references_go_to_the_original_copy(self):
        compiled, unit = self.weak_duplicate_unit()
        name = unit['weak_duplicates'][0]
        before, after = Elf32(compiled), Elf32(strip_objects(compiled, unit))
        defined = [s for s in after.symbols() if s['name'] == name + '$duplicate']
        self.assertEqual([(s['binding'], s['type']) for s in defined], [(2, 2)])
        self.assertEqual([s['section'] for s in after.symbols() if s['name'] == name], [0])
        # The kept functions' exception actions destroy templates with it.
        self.assertIn(name, [s['name'] for s in relocation_targets(before, 'extab')])
        targets = [s for s in relocation_targets(after, 'extab') if s['name'].startswith(name)]
        self.assertTrue(targets)
        self.assertTrue(all(s['name'] == name and not s['section'] for s in targets))
        self.assertEqual(before.contents(before.sections[1]), after.contents(after.sections[1]))

    def test_weak_duplicate_needs_a_retained_weak_original(self):
        _, unit = self.weak_duplicate_unit()
        validate_units(self.original, [unit])
        for mutate in (lambda u: u['externals'].pop(u['weak_duplicates'][0]),
                       lambda u: u.update(strip_unused=False),
                       lambda u: u['externals'].update(bsearch=u['externals']['bsearch'])
                       or u.update(weak_duplicates=['bsearch'])):
            mutated = json.loads(json.dumps(unit))
            mutate(mutated)
            with self.assertRaisesRegex(ValueError, 'Weak duplicate lacks one retained original copy|Function-symbol'):
                validate_units(self.original, [mutated])

    def test_weak_duplicate_must_be_compiled_weak(self):
        compiled, unit = self.weak_duplicate_unit()
        unit['weak_duplicates'] = ['GetMotionData__20CStaticObjectFactoryFii']
        with self.assertRaisesRegex(ValueError, 'not one compiled weak function'):
            strip_objects(compiled, unit)

    def weak_data_duplicate_unit(self):
        compiled = (self.build / 'units/collisionvolume_ctor/compiled.o').read_bytes()
        return compiled, json.loads((CONFIG / 'collisionvolume_ctor.json').read_text())

    def test_weak_data_duplicate_references_go_to_the_original_copy(self):
        compiled, unit = self.weak_data_duplicate_unit()
        name = '__vt__10ISceneNode'
        self.assertIn(name, unit['weak_duplicates'])
        self.assertEqual(unit['stripped_sections'], ['.data'])
        after = Elf32(strip_objects(compiled, unit))
        defined = [s for s in after.symbols() if s['name'] == name + '$duplicate']
        self.assertEqual([(s['binding'], s['type']) for s in defined], [(2, 1)])
        targets = [s for s in relocation_targets(after, '.text') if s['name'].startswith(name)]
        self.assertTrue(targets)
        self.assertTrue(all(s['name'] == name and not s['section'] for s in targets))
        validate_object(after, unit)

    def test_weak_data_duplicate_must_be_listed(self):
        compiled, unit = self.weak_data_duplicate_unit()
        unit['weak_duplicates'].remove('__vt__10ISceneNode')
        with self.assertRaisesRegex(ValueError, 'Unexpected compiler dependencies'):
            validate_object(Elf32(strip_objects(compiled, unit)), unit)

    def test_weak_data_duplicate_must_be_compiled_weak(self):
        compiled, unit = self.weak_data_duplicate_unit()
        unit['weak_duplicates'].append('@9')
        with self.assertRaisesRegex(ValueError, 'not one compiled weak function or data object'):
            strip_objects(compiled, unit)

    def global_vtable_duplicate_unit(self):
        compiled = (self.build / 'units/compartment_cptmaterial_dtor/compiled.o').read_bytes()
        return compiled, json.loads((CONFIG / 'compartment_cptmaterial_dtor.json').read_text())

    def test_global_vtable_duplicate_goes_to_the_weak_original(self):
        compiled, unit = self.global_vtable_duplicate_unit()
        name = '__vt__11CPTMaterial'
        compiled_table = [s for s in Elf32(compiled).symbols() if s['name'] == name and s['section']]
        self.assertEqual([s['binding'] for s in compiled_table], [1])  # compiled global
        self.assertEqual(unit['stripped_sections'], ['.data'])
        after = Elf32(strip_objects(compiled, unit))
        targets = [s for s in relocation_targets(after, '.text') if s['name'].startswith(name)]
        self.assertTrue(targets)
        self.assertTrue(all(s['name'] == name and not s['section'] for s in targets))
        validate_object(after, unit)

    def test_only_virtual_tables_may_be_compiled_global(self):
        compiled = (self.build / 'units/compartment_renderbin/compiled.o').read_bytes()
        unit = json.loads((CONFIG / 'compartment_renderbin.json').read_text())
        unit['weak_duplicates'] = ['__dt__17CTextureSwapCacheFv']  # a global function
        with self.assertRaisesRegex(ValueError, 'not one compiled weak function or data object'):
            strip_objects(compiled, unit)

    def test_global_vtable_duplicate_needs_a_weak_original(self):
        _, unit = self.global_vtable_duplicate_unit()
        unit['weak_duplicates'] = ['__vt__9CAIDoodad']  # global in the original
        unit['externals']['__vt__9CAIDoodad'] = '0x80183620'
        with self.assertRaisesRegex(ValueError, 'lacks one retained original copy'):
            validate_units(self.original, [unit])

    def test_unknown_object_fails(self):
        with self.assertRaisesRegex(ValueError, 'not one compiler object'):
            strip_objects(self.compiled, self.mutated('.sbss', ['NoSuchObject']))



class PoolReferences(unittest.TestCase):
    """player_sqrtf links its .sdata2 constants inside player.cpp's pool."""

    @classmethod
    def setUpClass(cls):
        path = ROOT / 'build/reconstruction/units/player_sqrtf/compiled.o'
        if not path.exists():
            raise unittest.SkipTest('Run reconstruct.py first')
        cls.original_path, cls.original = load_target()
        cls.unit = json.loads((CONFIG / 'player_sqrtf.json').read_text())
        cls.compiled = path.read_bytes()

    def copy(self):
        return json.loads(json.dumps(self.unit))

    def test_references_use_the_original_addresses(self):
        obj = Elf32(strip_objects(self.compiled, self.unit))
        targets = list(relocation_targets(obj, '.text'))
        names = sorted({s['name'] for s in targets if s['name'].startswith('__pool_')})
        self.assertEqual(names, ['__pool_80350540', '__pool_80350548', '__pool_80350550'])
        self.assertTrue(all(not s['section'] for s in targets))
        validate_object(obj, self.unit)

    def test_items_equal_the_original(self):
        validate_units(self.original, [self.unit])
        check_pool(self.original, Elf32(self.compiled), self.unit)

    def test_a_changed_constant_fails(self):
        obj = Elf32(self.compiled)
        section = next(s for s in obj.sections if s['name'] == '.sdata2')
        data = bytearray(self.compiled)
        data[section['offset'] + 16] ^= 1  # 3.0 becomes another double
        with self.assertRaisesRegex(ValueError, 'Pool item differs from the original'):
            check_pool(self.original, Elf32(bytes(data)), self.unit)

    def test_another_address_fails(self):
        unit = self.copy()
        unit['pool_references'][0]['address'] = '0x80350544'  # the 5.0f between them
        with self.assertRaisesRegex(ValueError, 'Pool item differs from the original'):
            check_pool(self.original, Elf32(self.compiled), unit)

    def test_a_partial_object_fails(self):
        unit = self.copy()
        unit['pool_references'][1]['size'] = 4
        with self.assertRaisesRegex(ValueError, 'Pool item size differs'):
            check_pool(self.original, Elf32(self.compiled), unit)

    def test_an_unlisted_reference_fails(self):
        unit = self.copy()
        unit['pool_references'].pop()
        with self.assertRaisesRegex(ValueError, 'pooled item that is not listed'):
            validate_object(Elf32(strip_objects(self.compiled, unit)), unit)

    def test_invalid_manifests_fail(self):
        mutations = [
            (lambda u: u['pool_references'][0].update(address='0x800a4d48'), 'outside the original section'),
            (lambda u: u['pool_references'][1].update(offset=12, size=8), 'overlap'),
            (lambda u: u['sections'].append({'name': '.sdata2', 'address': '0x80350540', 'size': 24, 'type': 1}),
             'not distinct'),
            (lambda u: u['pool_references'][0].update(section='.rodata'), 'Invalid pool reference'),
        ]
        for mutate, message in mutations:
            unit = self.copy()
            mutate(unit)
            with self.assertRaisesRegex(ValueError, message):
                validate_units(self.original, [unit])


class ZeroFillPoolReferences(unittest.TestCase):
    """dmesh_bins links its zero GXColor initializer to an entry of the .sbss2 pool."""

    @classmethod
    def setUpClass(cls):
        path = ROOT / 'build/reconstruction/units/dmesh_bins/compiled.o'
        if not path.exists():
            raise unittest.SkipTest('Run reconstruct.py first')
        cls.original_path, cls.original = load_target()
        cls.unit = json.loads((CONFIG / 'dmesh_bins.json').read_text())
        cls.compiled = path.read_bytes()

    def test_zero_fill_item_links_to_the_original_address(self):
        self.assertEqual(self.unit['pool_references'],
                         [{'section': '.sbss2', 'offset': 0, 'size': 4, 'address': '0x80351cc8'}])
        validate_units(self.original, [self.unit])
        check_pool(self.original, Elf32(self.compiled), self.unit)
        obj = Elf32(strip_objects(self.compiled, self.unit))
        names = {s['name'] for s in relocation_targets(obj, '.text') if s['name'].startswith('__pool_')}
        self.assertEqual(names, {'__pool_80351cc8'})

    def test_a_reference_outside_the_zero_fill_section_fails(self):
        unit = json.loads(json.dumps(self.unit))
        unit['pool_references'][0]['address'] = '0x80351cd0'  # past the 12-byte .sbss2
        with self.assertRaisesRegex(ValueError, 'outside the original section'):
            validate_units(self.original, [unit])


if __name__ == '__main__':
    unittest.main()
