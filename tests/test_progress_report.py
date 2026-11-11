"""Public reporting safeguards run without the original game or compilers."""
import copy
import json
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from progress_report import export, fingerprints
from setup import GAME_ID


class PublicProgress(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.config = self.root / "config" / GAME_ID
        self.config.mkdir(parents=True)
        (self.root / "src").mkdir()
        (self.root / "src/example.c").write_text("int example(void) { return 1; }\n")
        self.manifest = {"id": "example", "source": "src/example.c",
                         "category": "reconstructed_game",
                         "functions": [{"name": "example", "address": "0x80001000", "size": 8}]}
        self.write("example.json", self.manifest)
        self.write("project.json", {"units": ["example.json"],
                                   "progress": {"code_bytes": 100, "executable_sections": {".text": 100}}})
        self.write("baseline.json", {"sections": {".text": "0x80001000"}})
        self.snapshot = {"schema_version": 2, "verification": "local-complete-image-match",
                         "input_sha256": fingerprints(self.root),
                         "units": [{"id": "example", "category": "reconstructed_game",
                                    "functions": 1, "code_bytes": 8}],
                         "progress": {"matching_code_bytes": 8, "total_executable_code_bytes": 100,
                                      "matching_functions": 1,
                                      "categories": {"reconstructed_game": 8, "restored_library": 0}},
                         "code_map": {"schema_version": 1,
                                      "sections": [{"name": ".text", "address": "0x80001000", "size": 100}],
                                      "units": [{"name": "Unknown file/example", "ranges": [
                                          {"kind": "function", "name": "unfinished", "address": "0x80001008", "size": 92}]}]}}

    def write(self, name, value):
        (self.config / name).write_text(json.dumps(value))

    def test_context_preserves_whole_executable_denominator(self):
        report = export(self.snapshot, self.root)
        self.assertEqual(report["version"], 2)
        self.assertEqual(report["measures"]["matched_code_percent"], 8)
        self.assertEqual(sum(int(u["measures"]["total_code"]) for u in report["units"]), 100)
        self.assertEqual(report["units"][-1]["measures"]["matched_code"], "0")
        self.assertNotIn("total_functions", report["measures"])
        self.assertNotIn(str(self.root), json.dumps(report))

    def test_changed_source_is_stale(self):
        (self.root / "src/example.c").write_text("changed\n")
        with self.assertRaisesRegex(ValueError, "Stale"):
            export(self.snapshot, self.root)

    def test_added_source_is_stale(self):
        (self.root / "src/extra.c").write_text("new\n")
        with self.assertRaisesRegex(ValueError, "Stale"):
            export(self.snapshot, self.root)

    def test_removed_source_is_stale(self):
        (self.root / "src/example.c").unlink()
        with self.assertRaisesRegex(ValueError, "Stale"):
            export(self.snapshot, self.root)

    def use_tool(self):
        tools = self.root / "tools"
        tools.mkdir(exist_ok=True)
        (tools / "verifier.py").write_text("CHECK = 1\n")
        self.snapshot["input_sha256"] = fingerprints(self.root, ["tools/verifier.py"])

    def test_changed_recorded_tool_is_stale(self):
        self.use_tool()
        (self.root / "tools/verifier.py").write_text("CHECK = 2\n")
        with self.assertRaisesRegex(ValueError, "Stale"):
            export(self.snapshot, self.root)

    def test_removed_recorded_tool_is_stale(self):
        self.use_tool()
        (self.root / "tools/verifier.py").unlink()
        with self.assertRaisesRegex(ValueError, "Stale"):
            export(self.snapshot, self.root)

    def test_unrecorded_tool_changes_stay_fresh(self):
        self.use_tool()
        (self.root / "tools/docs_helper.py").write_text("print('docs')\n")
        export(self.snapshot, self.root)
        (self.root / "tools/docs_helper.py").write_text("print('changed')\n")
        export(self.snapshot, self.root)

    def test_loaded_tools_cover_imports_and_pins(self):
        # A fresh process, as in a real build: other tests import sync_docs here.
        import subprocess
        tools = Path(__file__).resolve().parents[1] / "tools"
        names = json.loads(subprocess.run(
            [sys.executable, "-c", "import json, progress_report, inputs; "
             "print(json.dumps(inputs.loaded_tools(progress_report.ROOT)))"],
            cwd=tools, capture_output=True, text=True, check=True).stdout)
        self.assertIn("tools/progress_report.py", names)
        self.assertIn("tools/inputs.py", names)
        self.assertIn("tools/toolchain.json", names)
        self.assertNotIn("tools/sync_docs.py", names)

    def test_inflated_snapshot_total_is_rejected(self):
        self.snapshot["progress"]["matching_code_bytes"] += 1
        with self.assertRaisesRegex(ValueError, "totals"):
            export(self.snapshot, self.root)

    def test_overlapping_functions_are_rejected(self):
        self.manifest["functions"].append(copy.deepcopy(self.manifest["functions"][0]))
        self.write("example.json", self.manifest)
        self.snapshot["input_sha256"] = fingerprints(self.root)
        self.snapshot["units"][0].update(functions=2, code_bytes=16)
        with self.assertRaisesRegex(ValueError, "Overlapping"):
            export(self.snapshot, self.root)

    def test_duplicate_units_are_rejected(self):
        self.snapshot["units"].append(copy.deepcopy(self.snapshot["units"][0]))
        with self.assertRaisesRegex(ValueError, "Duplicate"):
            export(self.snapshot, self.root)

    def test_map_cannot_double_count_matched_source(self):
        item = self.snapshot["code_map"]["units"][0]["ranges"][0]
        item.update(address="0x80001000", size=100)
        with self.assertRaisesRegex(ValueError, "coverage"):
            export(self.snapshot, self.root)

    def test_missing_map_bytes_are_rejected(self):
        self.snapshot["code_map"]["units"][0]["ranges"][0]["size"] -= 4
        with self.assertRaisesRegex(ValueError, "coverage"):
            export(self.snapshot, self.root)

    def test_map_cannot_move_the_original_section(self):
        self.snapshot["code_map"]["sections"][0]["address"] = "0x80002000"
        with self.assertRaisesRegex(ValueError, "pinned"):
            export(self.snapshot, self.root)

    def test_unidentified_bytes_are_not_reported_as_functions(self):
        self.snapshot["code_map"]["units"][0]["ranges"][0]["kind"] = "unidentified"
        unit = export(self.snapshot, self.root)["units"][-1]
        self.assertFalse(unit["functions"])
        self.assertEqual(unit["sections"][0]["size"], "92")
        self.assertEqual(unit["measures"]["matched_code"], "0")


if __name__ == "__main__":
    unittest.main()
