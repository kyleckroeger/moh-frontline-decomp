#!/usr/bin/env python3
"""Prepare pinned compiler candidates; choosing a version does not prove provenance."""
import argparse
import json
import zipfile
from pathlib import Path

from setup import ROOT, fetch, platform_spec

DEST = ROOT / "build/compiler"


def setup_compiler(version, family="ProDG"):
    if family not in ("ProDG", "GC"):
        raise ValueError("Unknown compiler family")
    lock = json.loads((ROOT / "tools/compilers.json").read_text())
    archive = DEST / "compilers.zip"
    fetch(lock["archive"], archive)
    wrapper = DEST / "wibo"
    fetch(platform_spec(lock["wibo"]), wrapper)  # macOS ARM64 runs it under Rosetta
    wrapper.chmod(0o755)
    with zipfile.ZipFile(archive) as package:
        versions = sorted({n.split("/")[1] for n in package.namelist() if n.startswith(f"{family}/") and n.count("/") >= 2})
        if version is None:
            return versions
        if version not in versions:
            raise ValueError(f"Unknown compiler {version}; available: {versions}")
        prefix = f"{family}/{version}/"
        directory = DEST / family / version
        for name in package.namelist():
            if not name.startswith(prefix) or name.endswith("/"):
                continue
            output = directory / name[len(prefix):]
            if not output.resolve().is_relative_to(directory.resolve()):
                raise ValueError("Unsafe compiler archive path")
            output.parent.mkdir(parents=True, exist_ok=True)
            data = package.read(name)
            # Rewriting an unchanged file fails while another process runs it.
            if not output.exists() or output.read_bytes() != data:
                output.write_bytes(data)
    return directory, wrapper


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--version")
    print(setup_compiler(parser.parse_args().version))
