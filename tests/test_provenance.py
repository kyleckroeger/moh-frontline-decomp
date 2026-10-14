"""Unit provenance must name real upstream sources, never this project's own templates."""
import json
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from port_unit import RISING_SUN, adapted_from

CONFIG = ROOT / "config" / "GMFE69"


class PortUnitProvenance(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.config = Path(self.tmp.name) / "config"
        self.config.mkdir()

    def tearDown(self):
        self.tmp.cleanup()

    def test_rising_sun_template_is_recorded(self):
        like = Path(self.tmp.name) / "rising-sun" / "unit.json"
        result = adapted_from(like, {"id": "unit"}, "other", self.config)
        self.assertEqual(result, {"repository": RISING_SUN, "manifest": "unit.json"})

    def test_own_template_is_not_provenance(self):
        like = self.config / "neighbour.json"
        self.assertIsNone(adapted_from(like, {"id": "neighbour"}, "unit", self.config))

    def test_reporting_a_unit_keeps_its_provenance(self):
        like = self.config / "unit.json"
        recorded = {"repository": RISING_SUN, "paths": ["src/a.cpp"]}
        reference = {"id": "unit", "adapted_from": recorded}
        self.assertEqual(adapted_from(like, reference, "unit", self.config), recorded)

    def test_reporting_a_unit_without_provenance_adds_none(self):
        like = self.config / "unit.json"
        self.assertIsNone(adapted_from(like, {"id": "unit"}, "unit", self.config))


class ManifestProvenance(unittest.TestCase):
    def test_adapted_from_names_upstream_sources(self):
        for path in sorted(CONFIG.glob("*.json")):
            if path.name == "project.json":
                continue
            unit = json.loads(path.read_text())
            adapted = unit.get("adapted_from")
            if adapted is None:
                continue
            with self.subTest(unit=path.name):
                self.assertEqual(adapted.get("repository"), RISING_SUN)
                if unit.get("category") == "reconstructed_game":
                    # Game units record the Rising Sun source files they follow.
                    paths = adapted.get("paths")
                    self.assertTrue(paths and all(isinstance(p, str) and p for p in paths))
                    self.assertNotIn("manifest", adapted)
                else:
                    # Library units copied from Rising Sun name the manifest they
                    # were copied from, which carries the same file name.
                    self.assertEqual(adapted.get("manifest"), path.name)
                    self.assertTrue(unit.get("upstream"))


if __name__ == "__main__":
    unittest.main()
