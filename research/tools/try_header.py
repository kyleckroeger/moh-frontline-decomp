#!/usr/bin/env python3
"""try_header.py: what moving every matched unit to a shared class header would cost.

PortReadiness step 3.3-3.4. For each unit whose source defines its own view of CLASS,
this builds a copy of the source with that definition (and the helper types and inline
functions the header now provides) replaced by #include "game/<CLASS>.h", then compiles,
links and verifies the unit exactly as research/tools/one_unit.py does. The unchanged
source is verified the same way first, as a control.

Nothing under src/ or config/ is changed: copies and builds go to scratch/try/<unit>/.

    try_header.py CLASS [--jobs 4] [--only UNIT...]
        writes research/layout/header_moves/CLASS.tsv: unit, source, status, detail

Statuses: kept (verified with the header), lost (compiles, doesn't verify),
no_compile (the header lacks something the file uses), control_fail (the unit doesn't
verify unchanged in this harness; not counted).
"""
import argparse, glob, json, os, re, shutil, sys, traceback
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

ROOT = Path(os.environ.get("TRY_HEADER_ROOT", Path(__file__).resolve().parents[2]))
CONFIG = ROOT / "config/GMFE69"
OUT = ROOT / "research/layout/header_moves"
# TRY_HEADER_DIR / TRY_HEADER_WORK let a header be tried before it is committed (a private
# scratch directory holds the draft header, the copies and the builds).
HEADER_DIR = Path(os.environ.get("TRY_HEADER_DIR", ROOT / "include"))
WORK = Path(os.environ.get("TRY_HEADER_WORK", ROOT / "scratch/try"))

# What a header provides besides its class, by class: removed from the moved copy too.
PROVIDES = {
    "CVector3": {"types": ["CVector3XYZW", "CVector3Data", "CVector3Pair"], "functions": ["sqrtf"]},
    # CMatrix.h includes CVector3.h: a unit already on CVector3.h may still define its helpers
    "CMatrix": {"types": ["CVector3XYZW", "CVector3Data", "CVector3Pair"]},
    # ISceneNode.h has the class's bases and includes CMatrix.h
    "ISceneNode": {"types": ["IDestructible", "ISubject", "IObserver", "CVector3XYZW", "CVector3Data", "CVector3Pair"],
                   "enums": ["EClsnId"]},
    # CScreen.h has CScreenFieldMode, the render mode record and includes CColor.h/CRect.h
    "CCamera": {"types": ["IMovingSceneNode", "CPlane", "IDestructible", "ISubject", "IObserver",
                          "CVector3XYZW", "CVector3Data", "CVector3Pair"], "enums": ["EClsnId"]},
    "CInputManager": {"types": ["CDeviceManager", "CGCDevice", "CGCActuatorView"]},
    "CPlayerObject": {"types": ["IMovingSceneNode", "CCamera", "CPlane", "IDestructible", "ISubject", "IObserver",
                                "CVector3XYZW", "CVector3Data", "CVector3Pair"], "enums": ["EClsnId", "EWeaponTypes"]},
    "CScreen": {"types": ["CColor", "CRect", "GXRenderModeObj", "_GXRenderModeObj", "CScreenFieldMode",
                          "CVector3XYZW", "CVector3Data", "CVector3Pair"],
                "enums": ["EScreenWidth", "EFrameFormat", "EDepthFormat"]},
}


EARLY_INCLUDE = {"CScreen": True, "CPlayerObject": True}


# Spellings of the class's fields that only some views have, rewritten to the header's.
# Only forms specific to the class: anything else fails to compile and is counted.
_XYZW = "xyzw"
REWRITES = {
    "CVector3": [
        # whole-union copies and double halves of the views' "d" member
        (r"([\w\]\[.>-]+?)\.d\s*=\s*([\w\]\[.>-]+?)\.d\s*;", r"*(CVector3Data*)&\1 = *(const CVector3Data*)&\2;"),
        (r"([\w\]\[.>-]+?)\.d\.pair\[([01])\]", r"((double*)&\1)[\2]"),
        (r"(?<![\w.>])d\.pair\[([01])\]", r"((double*)this)[\1]"),
        (r"\bd\.v\[([0-3])\]", lambda m: _XYZW[int(m[1])]),
        (r"\bd\.c\.([xyzw])\b", r"\1"),
        (r"(?<=[.>])c\.([xyzw])\b", r"\1"),
        (r"(?<![\w.>])c\.([xyzw])\b", r"\1"),
    ],
}


def block_end(text, brace):
    depth, i = 1, brace + 1
    while depth and i < len(text):
        depth += {"{": 1, "}": -1}.get(text[i], 0)
        i += 1
    return i


def type_span(text, name):
    """(start, end) of a top-level class/struct/union definition of name, through its ';'."""
    m = re.search(r"^(?:class|struct|union)\s+" + re.escape(name) + r"\b[^;{]*\{", text, re.M)
    if not m:
        return None
    end = block_end(text, m.end() - 1)
    semi = text.index(";", end)
    return m.start(), semi + 1


def function_span(text, name):
    m = re.search(r"^(?:extern\s+)?(?:inline\s+)?[\w\s\*&]+\b" + re.escape(name) + r"\s*\([^;{]*\)\s*\{", text, re.M)
    if not m:
        return None
    return m.start(), block_end(text, m.end() - 1)


def _arity(params):
    params = params.strip()
    return 0 if params in ("", "void") else params.count(",") + 1


ROWS = ["right", "front", "up", "pos"]
_EXPR = r"([\w\]\[]+(?:(?:\.|->)[\w\]\[]+)*)"  # a postfix chain: a.b->c[i]


def _cmatrix_view(out, view):
    """Rewrite a CMatrix view's field spellings to the header's rows right/front/up/pos."""
    fields = dict((n, t) for t, n in re.findall(r"^\s*((?:unsigned\s+)?\w+)\s+(\w+)(?:\[[^;]*)?;", view, re.M)
                  if t not in ("static", "return", "typedef", "friend"))
    names = {"forward": "front", "position": "pos", "m_right": "right", "m_front": "front", "m_up": "up", "m_pos": "pos"}
    for old, new in names.items():
        if old in fields:
            out = re.sub(r"(?<=\.|>)" + old + r"\b", new, out)
    decl = re.search(r"^\s*float\s+m\s*((?:\[\d+\])+)\s*;", view, re.M)
    if not decl:
        return out
    dims = [int(d) for d in re.findall(r"\d+", decl[1])]
    # bodies of out-of-line CMatrix members: there the bare m is the field
    spans = []
    for m in re.finditer(r"\bCMatrix::~?\w+\s*\([^;{]*\)\s*(?:const\s*)?\{", out):
        spans.append((m.end(), block_end(out, m.end() - 1)))

    def sub(pattern, repl, s):
        res, last = [], 0
        for a, b in spans:
            res.append(s[last:a]); res.append(re.sub(pattern, repl, s[a:b])); last = b
        res.append(s[last:])
        return "".join(res)

    if dims == [4, 4] or dims == [3, 4]:
        two = lambda pre: (lambda m: f"{pre(m)}{ROWS[int(m['i'])]}.{_XYZW[int(m['j'])]}")
        one = lambda pre: (lambda m: f"&{pre(m)}{ROWS[int(m['i'])]}.x")
        anyi = lambda pre: (lambda m: f"((float(*)[4])&{pre(m)}right)")
        q = lambda m: m["x"] + m["op"]
        bare = lambda m: ""
        out = re.sub(_EXPR.replace("(", "(?P<x>", 1) + r"(?P<op>\.|->)m\[(?P<i>\d)\]\[(?P<j>\d)\]", two(q), out)
        out = re.sub(_EXPR.replace("(", "(?P<x>", 1) + r"(?P<op>\.|->)m\[(?P<i>\d)\](?!\[)", one(q), out)
        out = re.sub(_EXPR.replace("(", "(?P<x>", 1) + r"(?P<op>\.|->)m\b(?!\w)", anyi(q), out)
        out = sub(r"(?<![\w.>])m\[(?P<i>\d)\]\[(?P<j>\d)\]", two(bare), out)
        out = sub(r"(?<![\w.>])m\[(?P<i>\d)\](?!\[)", one(bare), out)
        out = sub(r"(?<![\w.>&])m\b(?!\w)", anyi(bare), out)
    elif dims == [16]:
        flat = lambda pre: (lambda m: f"{pre(m)}{ROWS[int(m['k']) // 4]}.{_XYZW[int(m['k']) % 4]}")
        anyk = lambda pre: (lambda m: f"((float*)&{pre(m)}right)")
        q = lambda m: m["x"] + m["op"]
        bare = lambda m: ""
        out = re.sub(_EXPR.replace("(", "(?P<x>", 1) + r"(?P<op>\.|->)m\[(?P<k>1[0-5]|\d)\]", flat(q), out)
        out = re.sub(_EXPR.replace("(", "(?P<x>", 1) + r"(?P<op>\.|->)m\b(?!\w)", anyk(q), out)
        out = sub(r"(?<![\w.>])m\[(?P<k>1[0-5]|\d)\]", flat(bare), out)
        out = sub(r"(?<![\w.>&])m\b(?!\w)", anyk(bare), out)
    return out


# The original's slot names, by class (research/layout/vtables.tsv, primary part).
def _slots(cls):
    names = []
    for line in (ROOT / "research/layout/vtables.tsv").read_text().splitlines()[1:]:
        c = line.split("\t")
        if c[1] == cls and c[5] == "0" and c[6] == "slot":
            m = re.match(r"(\w+)__", c[8]) if c[8] != "0" else None
            names.append(m[1] if m else None)
    return names


def _fields(view):
    return set(n for t, n in re.findall(r"^\s*((?:unsigned\s+)?[\w:]+\s*\**)\s*(\w+)(?:\[[^;]*)?;", view, re.M))


def _rename_access(out, old, new):
    return re.sub(r"(?<=\.|>)" + old + r"\b", new, out)


def _virtual_names(out, view, cls, nbase=0):
    """A view's virtuals renamed to the original's names by slot (views without bases, or with
    nbase inherited slots): calls through . and -> are renamed."""
    slots = _slots(cls)
    decl = re.findall(r"^\s*virtual\s+[^;{(]*?(~?\w+)\s*\(", view, re.M)
    for i, name in enumerate(decl):
        k = nbase + i
        if name.startswith("~") or k >= len(slots) or not slots[k] or slots[k] == name:
            continue
        if name in [n for n in decl if n == slots[k]]:
            continue
        out = re.sub(r"(?<=\.|>)" + name + r"(?=\s*\()", slots[k], out)
    return out


def _cscreen_view(out, view):
    fields = _fields(view)
    names = {"m_scaleX": "m_widthScale", "m_scaleY": "m_heightScale", "m_scale": "m_heightScale",
             "m_frontBuffer": "m_copyBuffer"}
    for old, new in names.items():
        if old in fields:
            out = _rename_access(out, old, new)
    out = re.sub(r"^unsigned long CScreen::GetVSyncs\(\)", "int CScreen::GetVSyncs()", out, flags=re.M)
    out = re.sub(r"^extern\s+CScreen\s+g_screen\s*;", "extern CScreenFieldMode g_screen;", out, flags=re.M)
    if not re.search(r"^\s*(class|struct)\s+CScreen\s*:", view, re.M):
        out = _virtual_names(out, view, "CScreen")
    return out


_VIEWS = None


def _lint_view(path, cls):
    """The file's view of cls as the layout lint read it (build/layout/views.json): {offset: name}."""
    global _VIEWS
    if _VIEWS is None:
        p = ROOT / "build/layout/views.json"
        _VIEWS = json.loads(p.read_text())["views"] if p.exists() else {}
    for v in _VIEWS.get(path, []):
        if v["name"] == cls and v["members"]:
            return {m[1]: m[0] for m in v["members"] if m[2] is None}
    return {}


def _lint_bits(path, cls):
    """The view's one-bit bitfields: {(offset, bit): name}."""
    _lint_view(path, cls)
    for v in _VIEWS.get(path, []):
        if v["name"] == cls and v["members"]:
            return {(m[1], m[2]): m[0] for m in v["members"] if m[2] is not None and m[3] == 1}
    return {}


_PLACEHOLDER = re.compile(r"^(unknown|pad|field|unk|data|_)[0-9a-fA-F_]*$")


def _offset_names(out, view, cls, path):
    """Fields the view names differently from the header at the same offset: accesses through
    . and -> renamed, and bare names inside the class's out-of-line member bodies."""
    want = dict(OFFSET_NAMES.get(cls, {}))
    have = _lint_view(path, cls)
    for (off, bit), name in _lint_bits(path, cls).items():
        if (off, bit) in OFFSET_BITS.get(cls, {}):
            have[(off, bit)] = name
            want[(off, bit)] = OFFSET_BITS[cls][(off, bit)]
    for off, name in have.items():
        new = want.get(off)
        if not new or new == name or _PLACEHOLDER.match(name) or name.startswith("__vptr"):
            continue
        out = _rename_access(out, name, new)
        spans = [(m.end(), block_end(out, m.end() - 1)) for m in
                 re.finditer(r"\b" + cls + r"::~?\w+\s*\([^;{]*\)\s*(?:const\s*)?(?::[^{;]*)?\{", out)]
        for a, b in reversed(spans):
            out = out[:a] + re.sub(r"(?<![\w.>])" + name + r"\b", new, out[a:b]) + out[b:]
    return out


# One-bit flags by (offset, bit from the most significant), for _offset_names.
OFFSET_BITS = {"CPlayerObject": {(o, i): n for o, ns in (
    (0x394, ["m_landed", "m_onFloor", "m_onGround", "m_collided", "m_aiming", "m_flag394_04", "m_flag394_02", "m_climbing"]),
    (0x395, ["m_crouched", "m_crouchStarted", "m_cameraShaking", "m_peeking", "m_onPath", "m_forceApplied", "m_flag395_02", "m_flag395_01"]),
    (0x396, ["m_usingMountedWeapon", "m_noShells", "m_peekStarted", "m_mgIsHier", "m_mountingMG", "m_mgHasWeapon", "m_noExplosionPush", "m_flag396_01"]),
    (0x397, ["m_weaponHidden", "m_dead", "m_fallingDamage", "m_flag397_10", "m_canCrouch", "m_motionShaking", "m_flag397_02", "m_wielding"]),
    (0x398, ["m_triggerChecks", "m_superAim", "m_flag398_20", "m_flag398_10", "m_flag398_08", "m_flag398_04", "m_flag398_02", "m_flag398_01"]),
) for i, n in enumerate(ns)}}


# The header's field names by offset, for _offset_names.
OFFSET_NAMES = {
    "CPlayerObject": {0x24: "m_doodad", 0x78: "m_value078", 0x7c: "m_value07c", 0x80: "m_tm", 0xc0: "m_cameraTM", 0x100: "m_localVolume3", 0x104: "m_worldVolume3", 0x110: "m_boxA", 0x160: "m_boxB", 0x1b0: "m_localCapsule", 0x200: "m_worldCapsule", 0x250: "m_script", 0x258: "m_velocity", 0x268: "m_localVelocity", 0x278: "m_force", 0x288: "m_position", 0x298: "m_crouchHeight", 0x29c: "m_yaw", 0x2a0: "m_pitch", 0x2a4: "m_heightSpeed", 0x2a8: "m_collisions", 0x2c0: "m_hearing", 0x390: "m_value390", 0x39c: "m_footstepTimer", 0x3a0: "m_cameraShakeStrength", 0x3a4: "m_cameraShakeRate", 0x3a8: "m_cameraShakeTime", 0x3ac: "m_motionShakeStrength", 0x3b0: "m_motionShakeTime", 0x3b4: "m_motionShakeElapsed", 0x3b8: "m_deathTime", 0x3c0: "m_cameraForward", 0x3e0: "m_lastGroundPosition", 0x3f0: "m_peekRight", 0x400: "m_path", 0x404: "m_nextPath", 0x408: "m_pathTime", 0x40c: "m_pathRate", 0x410: "m_fallStartZ", 0x414: "m_mgYawLimit", 0x418: "m_mgPitchLimit", 0x41c: "m_mgYaw", 0x420: "m_mgPitch", 0x42c: "m_fov", 0x430: "m_zoomFov", 0x434: "m_targetFov", 0x438: "m_fovSpeed", 0x43c: "m_muzzleFlash", 0x440: "m_recoilYaw", 0x444: "m_recoilPitch", 0x448: "m_field448", 0x44c: "m_peek", 0x450: "m_duck", 0x454: "m_fireDelay", 0x458: "m_grenadeHoldTime", 0x45c: "m_currentWeapon", 0x460: "m_weapons", 0x514: "m_soldierWeapons", 0x5c8: "m_mgObject", 0x5cc: "m_weaponNode", 0x5d0: "m_lastMG", 0x5d4: "m_mgWeaponA", 0x5d8: "m_mgWeaponB", 0x5dc: "m_lastWeapon", 0x5e0: "m_mountedWeapon", 0x5f0: "m_flashMatrix", 0x630: "m_mountMatrix", 0x670: "m_mgSaved", 0x6b0: "m_mgCollisionId", 0x6b4: "m_turnScale", 0x6b8: "m_savedClipSize", 0x6cc: "m_collisionId", 0x6d0: "m_flashScale", 0x6e0: "m_mgDirection", 0x6f0: "m_mgIndex", 0x6f4: "m_crossHair", 0x6f8: "m_throwHelmet", 0x700: "m_handMatrix", 0x740: "m_scriptPosition", 0x750: "m_soundHandle", 0x752: "m_landTimer", 0x754: "m_fireValue", 0x758: "m_delta", 0x75c: "m_fallScale", 0x760: "m_value760", 0x764: "m_controller", 0x768: "m_viewport", 0x76c: "m_playerIndex", 0x770: "m_scriptObject", 0x7b0: "m_speedState", 0x7b4: "m_recoil", 0x7b8: "m_lastPitch", 0x7bc: "m_recoilStep", 0x7c0: "m_value7c0", 0x7c4: "m_value7c4", 0x7c8: "m_soldier", 0x7d0: "m_camera", 0xa70: "m_ui", 0x1234: "m_unknown1234", 0x1270: "m_showUI", 0x1288: "m_viewport1288", 0x128c: "m_timeRemaining", 0x1290: "m_timeFlash", 0x1298: "m_value1298", 0x129a: "m_name", 0x12b0: "m_team", 0x12b4: "m_jumping", 0x12b8: "m_jumpTimer", 0x12bc: "m_startYaw", 0x12c0: "m_startPitch"},
    "CScene": {0x0: "m_head", 0x4: "m_tail", 0x8: "m_players", 0x18: "m_world", 0x1c: "m_camera",
               0x20: "m_staticHead", 0x24: "m_staticTail", 0x28: "m_animHead", 0x2c: "m_animTail",
               0x30: "m_lightHead", 0x34: "m_lightTail", 0x38: "m_nodeCount", 0x3c: "m_staticCount",
               0x40: "m_animCount", 0x44: "m_lightCount", 0x48: "m_updating", 0x4c: "m_playerCount",
               0x50: "m_lights", 0x68: "m_shadows", 0x80: "m_extentsX", 0xac: "m_extentsY", 0xd8: "m_pairsX",
               0xf0: "m_pairsY", 0x108: "m_collisions", 0x118: "m_exclusions", 0x130: "m_commands"},
    "CInputManager": {0x2430: "m_keymap", 0x2434: "m_playerKeymaps", 0x2444: "field2444",
                      0x2445: "m_feedback", 0x2446: "m_feedbackOff", 0x244a: "m_inverted",
                      0x244b: "m_zooming"},
    "CCamera": {0x24: "m_near", 0x28: "m_far", 0x2c: "m_width", 0x30: "m_height", 0x40: "m_tm",
                0x80: "m_worldToCamera", 0xc0: "m_projection", 0x100: "m_worldToScreen",
                0x140: "m_perspective", 0x141: "m_flag141", 0x142: "m_projectionValid",
                0x143: "m_flag143", 0x150: "m_planes", 0x290: "m_script", 0x294: "data294"},
}


def _ccamera_view(out, view, path=None):
    if path:
        out = _offset_names(out, view, "CCamera", path)
    if not re.search(r"^\s*(class|struct)\s+CCamera\s*:", view, re.M):
        out = _virtual_names(out, view, "CCamera")
    return out


def _cinputmanager_view(out, view, path=None):
    out = re.sub(r"^int CInputManager::VerifyAnalog\(\)", "bool CInputManager::VerifyAnalog()", out, flags=re.M)
    return _offset_names(out, view, "CInputManager", path) if path else out


VIEW_REWRITES = {"CMatrix": _cmatrix_view, "CScreen": _cscreen_view, "CCamera": _ccamera_view,
                 "CInputManager": _cinputmanager_view}
VIEW_REWRITES["CScene"] = lambda out, view, path=None: _offset_names(out, view, "CScene", path) if path else out
def _cplayerobject_view(out, view, path=None):
    if path:
        out = _offset_names(out, view, "CPlayerObject", path)
    if not re.search(r"^\s*(class|struct)\s+CPlayerObject\s*:", view, re.M):
        out = _virtual_names(out, view, "CPlayerObject")
    else:
        for old, new in (("GetInventory", "GetScriptObject"), ("SetScriptObject", "SetScript")):
            if re.search(r"\bvirtual\b[^;]*\b" + old + r"\s*\(", view):
                out = re.sub(r"(?<=\.|>)" + old + r"(?=\s*\()", new, out)
    return out


VIEW_REWRITES["CPlayerObject"] = _cplayerobject_view
VIEW_PATH = {"CCamera", "CInputManager", "CScene", "CPlayerObject"}


def moved(text, cls, path=None):
    span = type_span(text, cls)
    if not span:
        return None
    MARK = "\x00INCLUDE\x00"
    out = text[:span[0]] + MARK + text[span[1]:]
    extra = PROVIDES.get(cls, {})
    first = span[0]
    for name in extra.get("types", []):
        while True:
            s = type_span(out, name)
            if not s:
                break
            first = min(first, s[0])
            out = out[:s[0]] + out[s[1]:]
    for name in extra.get("enums", []):
        m = re.search(r"^enum\s+" + re.escape(name) + r"\s*\{[^}]*\}\s*;[ \t]*\n?", out, flags=re.M)
        if m:
            first = min(first, m.start())
        out = re.sub(r"^enum\s+" + re.escape(name) + r"\s*\{[^}]*\}\s*;[ \t]*\n?", "", out, flags=re.M)
    inc = f'#include "game/{cls}.h"\n'
    if EARLY_INCLUDE.get(cls) and out.index(MARK) > first:
        # the header goes where the first type it provides was (the file uses it before the view)
        out = out.replace(MARK, "")
        first = min(first, len(out))
        # first is a position in the text before later removals; back up to a line start
        first = out.rfind("\n", 0, first) + 1 if first else 0
        out = out[:first] + inc + out[first:]
    else:
        out = out.replace(MARK, inc)
    for name in extra.get("functions", []):
        s = function_span(out, name)
        if s:
            out = out[:s[0]] + out[s[1]:]
    for pattern, repl in REWRITES.get(cls, []):
        out = re.sub(pattern, repl, out)
    if cls in VIEW_REWRITES:
        if cls in VIEW_PATH:
            out = VIEW_REWRITES[cls](out, text[span[0]:span[1]], path)
        else:
            out = VIEW_REWRITES[cls](out, text[span[0]:span[1]])
    # Out-of-line definitions of the class's methods the header now defines inline
    # (units emit the original's weak copies this way): the header's body replaces them.
    header = (HEADER_DIR / f"game/{cls}.h").read_text()
    inline = set(re.findall(r"^\s+(?:[\w&\*]+\s+)*?(\w+|operator\S+?)\s*\(([^;{]*)\)\s*(?:const\s*)?(?::[^{;]*)?\{", header, re.M))
    inline = {(n, _arity(p)) for n, p in inline}
    pat = re.compile(r"^(?:__declspec\(weak\)\s+)?(?:inline\s+)?[\w\s&\*]*?\b" + re.escape(cls)
                     + r"::(~?\w+|operator\S+?)\s*\(([^;{]*)\)\s*(?:const\s*)?(?::[^{;]*)?\{", re.M)
    while True:
        # by name and parameter count: an inline overload doesn't remove an out-of-line one
        m = next((m for m in pat.finditer(out) if (m[1], _arity(m[2])) in inline), None)
        if not m:
            break
        out = out[:m.start()] + out[block_end(out, m.end() - 1):]
    return out


def verify(unit, work):
    """one_unit.py's flow, for a unit dict and work directory; returns (ok, detail)."""
    sys.path.insert(0, str(ROOT / "tools"))
    import project_build as pb
    from audit import load_target
    from formats import Elf32
    from setup import setup
    from setup_compiler import setup_compiler
    from sdk_build import compile_sdk, sdk_link_script
    from tool_runner import run
    from data_strip import strip_objects
    from extab_patch import apply_patches, load_patches
    uid = unit["id"]
    _, original = load_target()
    tools = setup()
    compiler, wrapper = setup_compiler(unit["compiler_version"], "GC")
    linker, _ = setup_compiler("3.8.1", "ProDG")
    shutil.rmtree(work, ignore_errors=True)
    work.mkdir(parents=True)
    env = os.environ.copy()
    env["SN_NGC_PATH"] = "Z:" + str(compiler).replace("/", "\\")

    def execute(stage, *args):
        run(args, work, work / f"{stage}.log", env)
    try:
        compile_sdk(unit, compiler, wrapper, work, execute)
    except Exception:
        log = (work / "compile.log").read_text(errors="replace") if (work / "compile.log").exists() else ""
        lines = [l.strip() for l in log.splitlines() if l.strip().startswith("#") and "Error" not in l]
        return "no_compile", " ".join(lines[:4])[:300]
    try:
        pb.check_pool(original, Elf32((work / "compiled.o").read_bytes()), unit)
        units = [json.loads((CONFIG / n).read_text()) for n in json.loads((CONFIG / "project.json").read_text())["units"]]
        patches = load_patches([u for u in units if u["id"] != uid] + [unit]).get(uid, [])
        (work / "input.o").write_bytes(strip_objects(apply_patches((work / "compiled.o").read_bytes(), patches), unit))
        obj = Elf32((work / "input.o").read_bytes())
        pb.validate_object(obj, unit)
        (work / "source.ld").write_text(sdk_link_script(unit, obj, original, tools, work, execute))
        kept_text = [(f["section"], f["address"], f["address"] + f["size"]) for f in obj.symbols()
                     if f["type"] == 2 and f["name"] not in unit["discarded_functions"]
                     and obj.sections[f["section"]]["name"] in pb.CODE_SECTIONS] if unit["strip_unused"] else []
        roots = [f["name"] for f in unit["functions"] + unit.get("label_functions", []) if f["binding"]] + sorted(
            {s["name"] for s in obj.symbols() if s["binding"] and s["type"] == 0 and s["section"]
             and s["section"] < 0xFF00 and obj.sections[s["section"]]["name"] in pb.CODE_SECTIONS
             and any(i == s["section"] and a <= s["address"] < b for i, a, b in kept_text)})
        strip_args = ["--strip-unused"] + [a for n in roots for a in ("--undefined", n)] if unit["strip_unused"] else []
        if unit.get("retain_function_symbols", False):
            (work / "functions.keep").write_text("".join(f["name"] + "\n" for f in unit["functions"] + unit.get("label_functions", [])))
            strip_args += ["--retain-symbols-file", "functions.keep"]
        execute("link", wrapper, linker / "ngcld.exe", *strip_args, "--fix-sda", "-T", "source.ld", "-o", "compiled.elf")
        pb.verify_unit(original, Elf32((work / "compiled.elf").read_bytes()), unit)
        return "kept", ""
    except Exception as e:
        return "lost", " ".join(str(e).split())[:300]


def attempt(job):
    uid, cls = job
    unit = json.loads((CONFIG / f"{uid}.json").read_text())
    base = WORK / uid
    status, detail = verify(unit, base / "control")
    if status != "kept":
        return uid, unit["source"], "control_fail", detail
    text = (ROOT / unit["source"]).read_text(errors="replace")
    new = moved(text, cls, unit["source"])
    copy = base / "src" / Path(unit["source"]).name
    copy.parent.mkdir(parents=True, exist_ok=True)
    copy.write_text(new)
    unit = dict(unit, source=str(copy.relative_to(ROOT)), include_dirs=([str(HEADER_DIR.relative_to(ROOT))] if HEADER_DIR != ROOT / "include" else []) + unit["include_dirs"] + ["include"])
    try:
        status, detail = verify(unit, base / "moved")
    except Exception:
        status, detail = "error", traceback.format_exc().splitlines()[-1]
    return uid, unit["source"], status, detail


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("cls")
    ap.add_argument("--jobs", type=int, default=4)
    ap.add_argument("--only", nargs="*")
    a = ap.parse_args()
    units = json.loads((CONFIG / "project.json").read_text())["units"]
    pat = re.compile(r"^(?:class|struct)\s+" + re.escape(a.cls) + r"\b[^;{]*\{", re.M)
    jobs = []
    for name in units:
        uid = name[:-5] if name.endswith(".json") else name
        unit = json.loads((CONFIG / f"{uid}.json").read_text())
        src = ROOT / unit["source"]
        if src.exists() and pat.search(src.read_text(errors="replace")) and (not a.only or uid in a.only):
            jobs.append((uid, a.cls))
    print(f"{len(jobs)} units define their own {a.cls}", flush=True)
    rows = []
    with ProcessPoolExecutor(a.jobs) as pool:
        for row in pool.map(attempt, jobs):
            rows.append(row)
            print("\t".join(row[:3]), flush=True)
    outdir = Path(os.environ.get("TRY_HEADER_OUT", OUT))
    outdir.mkdir(parents=True, exist_ok=True)
    out = outdir / f"{a.cls}.tsv"
    if a.only and out.exists():  # merge a partial rerun into the full table
        old = {r.split("\t")[0]: r for r in out.read_text().splitlines()[1:]}
        for r in rows:
            old[r[0]] = "\t".join(r)
        lines = sorted(old.values())
    else:
        lines = sorted("\t".join(r) for r in rows)
    out.write_text("unit\tsource\tstatus\tdetail\n" + "\n".join(lines) + "\n")
    from collections import Counter
    print(Counter(l.split("\t")[2] for l in lines))


if __name__ == "__main__":
    main()
