"""Input fingerprints shared by the source build and the public progress export.

Every file in src/, include/ and config/ is fingerprinted, so any change, addition
or removal there makes a snapshot stale. In tools/, only the files a verification
run depended on are recorded: the Python modules it imported plus every non-Python
file (compiler and toolchain pins). Editing or adding a tool that verification
never loads (docs sync, research helpers) therefore leaves the snapshot fresh,
while editing one it does load still forces a rebuild and recapture.
"""
import hashlib
import sys
from pathlib import Path

FOLDERS = ("src", "include", "config")


def _digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _usable(path):
    return path.is_file() and "__pycache__" not in path.parts


def loaded_tools(root):
    """Repo-relative tools/ files the running process depends on."""
    root = Path(root).resolve()
    tools = root / "tools"
    paths = {Path(m.__file__).resolve() for m in list(sys.modules.values())
             if getattr(m, "__file__", None)}
    paths |= {p.resolve() for p in tools.rglob("*") if p.suffix != ".py"}
    return sorted(p.relative_to(root).as_posix() for p in paths
                  if p.is_relative_to(tools) and _usable(p))


def fingerprints(root, tool_files=()):
    """SHA-256 of every file in FOLDERS plus the named tools/ files."""
    root = Path(root)
    files = [p for folder in FOLDERS for p in sorted((root / folder).rglob("*")) if _usable(p)]
    files += [root / name for name in tool_files]
    return {p.relative_to(root).as_posix(): _digest(p) for p in files}


def is_fresh(recorded, root):
    """True when the tree still matches a recorded fingerprint set."""
    tools = [name for name in recorded if name.startswith("tools/")]
    if not all((Path(root) / name).is_file() for name in tools):
        return False
    return recorded == fingerprints(root, tools)
