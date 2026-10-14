#!/usr/bin/env python3
"""Install checksum-pinned analysis tools locally; no system changes."""
import hashlib
import json
import platform
import shutil
import urllib.request
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GAME_ID = "GMFE69"
CONFIG = ROOT / "config" / GAME_ID
ORIG = ROOT / "orig" / GAME_ID
DEST = ROOT / "build/tools"


def fetch(spec, output):
    output.parent.mkdir(parents=True, exist_ok=True)
    if output.exists() and hashlib.sha256(output.read_bytes()).hexdigest() == spec["sha256"]:
        return
    temporary = output.with_suffix(".download")
    with urllib.request.urlopen(spec["url"], timeout=120) as response, temporary.open("wb") as stream:
        shutil.copyfileobj(response, stream)
    if hashlib.sha256(temporary.read_bytes()).hexdigest() != spec["sha256"]:
        temporary.unlink()
        raise RuntimeError(f"Checksum mismatch: {output.name}")
    temporary.replace(output)


def host_platform():
    """The pinned-tool key for this machine, e.g. "Darwin-arm64" or "Linux-x86_64"."""
    machine = platform.machine()
    return f"{platform.system()}-{'x86_64' if machine == 'AMD64' else machine}"


def platform_spec(tool):
    """The download (url, sha256) of a pinned tool for this machine."""
    key = host_platform()
    if key not in tool["platforms"]:
        raise RuntimeError(f"No pinned tools for {key}; supported: {', '.join(sorted(tool['platforms']))}")
    return tool["platforms"][key]


def setup():
    lock = json.loads((ROOT / "tools/toolchain.json").read_text())
    fetch(platform_spec(lock["dtk"]), DEST / "dtk")
    (DEST / "dtk").chmod(0o755)
    archive = DEST / "binutils.zip"
    fetch(platform_spec(lock["binutils"]), archive)
    with zipfile.ZipFile(archive) as package:
        for name in ("powerpc-eabi-as", "powerpc-eabi-ld", "powerpc-eabi-objdump", "powerpc-eabi-readelf", "powerpc-eabi-objcopy", "powerpc-eabi-nm"):
            path = DEST / name
            data = package.read(name)
            # Rewriting an unchanged tool fails while another process runs it.
            if not path.exists() or path.read_bytes() != data:
                path.write_bytes(data)
            path.chmod(0o755)
    return DEST


if __name__ == "__main__":
    print(setup())
