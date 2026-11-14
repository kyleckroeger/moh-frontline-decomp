"""mkvar.py: write source variants (one device removed each) to scratch/loop3/audit1/var/<name>/<file>; prints 'name unit path'."""
import re, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[3]
OUT = ROOT / "scratch/loop3/audit1/var"

def var(name, unit, src, pairs):
    text = (ROOT / src).read_text()
    for old, new in pairs:
        if old not in text:
            sys.exit(f"{name}: missing {old!r}")
        text = text.replace(old, new)
    d = OUT / name
    d.mkdir(parents=True, exist_ok=True)
    p = d / Path(src).name
    p.write_text(text)
    print(name, unit, p.relative_to(ROOT))

var("thrown_nochain", "thrown_obj_init", "src/game/thrown_obj_init.cpp", [
    ("m_vector288[0] = m_vector288[1] = m_vector288[2] = 0.0f;", "m_vector288[0] = 0.0f; m_vector288[1] = 0.0f; m_vector288[2] = 0.0f;"),
    ("m_vector2a8[0] = m_vector2a8[1] = m_vector2a8[2] = 0.0f;", "m_vector2a8[0] = 0.0f; m_vector2a8[1] = 0.0f; m_vector2a8[2] = 0.0f;"),
    ("m_value2e4 = m_value2e8 = 0;", "m_value2e4 = 0; m_value2e8 = 0;"),
    ("m_value2ec = m_value2f0 = 0;", "m_value2ec = 0; m_value2f0 = 0;")])
var("thrown_nochain_rev", "thrown_obj_init", "src/game/thrown_obj_init.cpp", [
    ("m_vector288[0] = m_vector288[1] = m_vector288[2] = 0.0f;", "m_vector288[2] = 0.0f; m_vector288[1] = 0.0f; m_vector288[0] = 0.0f;"),
    ("m_vector2a8[0] = m_vector2a8[1] = m_vector2a8[2] = 0.0f;", "m_vector2a8[2] = 0.0f; m_vector2a8[1] = 0.0f; m_vector2a8[0] = 0.0f;"),
    ("m_value2e4 = m_value2e8 = 0;", "m_value2e8 = 0; m_value2e4 = 0;"),
    ("m_value2ec = m_value2f0 = 0;", "m_value2f0 = 0; m_value2ec = 0;")])
var("sound_nochain", "sound", "src/game/sound.cpp", [
    ("listener.d.v[0] = listener.d.v[1] = listener.d.v[2] = 0.0f;", "{ listener.d.v[2] = 0.0f; listener.d.v[1] = 0.0f; listener.d.v[0] = 0.0f; }")])
var("sound_nochain_fwd", "sound", "src/game/sound.cpp", [
    ("listener.d.v[0] = listener.d.v[1] = listener.d.v[2] = 0.0f;", "{ listener.d.v[0] = 0.0f; listener.d.v[1] = 0.0f; listener.d.v[2] = 0.0f; }")])
var("capsule_nowrap", "capsule_plane", "src/game/capsule_plane.cpp", [
    ("inline float SubV(float a, float b) { return a - b; }\ninline float AddV(float a, float b) { return a + b; }\n", "")]
    + [(f"{f}V({c}.{x}, radius)", f"{c}.{x} {'-' if f=='Sub' else '+'} radius") for f in ("Sub", "Add") for c in "ab" for x in "xyz"])
var("font_nomul", "font_centeredf", "src/game/font_centeredf.cpp", [
    ("float width = Mul((float)screen->GetWidth(), viewWidth);", "float width = (float)screen->GetWidth() * viewWidth;")])
var("font_nocurrent", "font_centeredf", "src/game/font_centeredf.cpp", [
    ("CScreen* screen = CurrentScreen();", "CScreen* screen = CScreen::GetCurrent();")])
var("buddy_noiterfwd", "buddy_head", "src/game/buddy_head.cpp", [
    ("    GiveBuddyOrders(it);\n}", "    GiveBuddyOrders(it.operator->());\n}"),
    ("            GiveBuddyOrders(it);\n", "            GiveBuddyOrders(it.operator->());\n")])
var("buddy_nodumpvec", "buddy_head", "src/game/buddy_head.cpp", [
    ("inline void DumpBuddyVector(dwi::vector<BuddyInfo>& buddies) {", "inline void DumpAttachedBuddies() {\n    dwi::vector<BuddyInfo>& buddies = g_attached_buddies;"),
    ("inline void DumpAttachedBuddies() {\n    DumpBuddyVector(g_attached_buddies);\n}\n", "")])
var("buddy_nodumpvec2", "buddy_head", "src/game/buddy_head.cpp", [
    ("inline void DumpBuddyVector(dwi::vector<BuddyInfo>& buddies) {", "inline void DumpAttachedBuddies() {"),
    ("buddies.m_size", "g_attached_buddies.m_size"), ("buddies[i]", "g_attached_buddies[i]"),
    ("inline void DumpAttachedBuddies() {\n    DumpBuddyVector(g_attached_buddies);\n}\n", "")])
var("cpt_noswapvcd", "compartment_init", "src/game/compartment_init.cpp", [
    ("    WriteSwapVCD(list, p);", "    WriteVCD(list, p);")])
var("cpt_nopalette", "compartment_init", "src/game/compartment_init.cpp", [
    ("if (palette->m_type == 0x32) palette = (PaletteHeader*)((char*)header + paletteOffset); else palette = 0;", "if (palette->m_type != 0x32) palette = 0;")])
var("cpt_nobinb", "compartment_init", "src/game/compartment_init.cpp", [
    ("            b->m_global = g_GlobalCpt.m_data->m_simple + (int)b->m_global;\n        bin = b;", "            b->m_global = g_GlobalCpt.m_data->m_simple + (int)b->m_global;"),
    ("            b->m_global = g_GlobalCpt.m_data->m_alpha + (int)b->m_global;\n        bin = b;", "            b->m_global = g_GlobalCpt.m_data->m_alpha + (int)b->m_global;")])
var("fastvec_nosize2", "compartment_fastvec_tri", "src/game/compartment_fastvec_tri.cpp", [
    ("m_owned(false), m_growth(growth) {\n        m_size = 0;\n    }", "m_owned(false), m_growth(growth) {}")])
var("fastvec_realnames", "compartment_fastvec_tri", "src/game/compartment_fastvec_tri.cpp", [
    ("    bool SwapCollisionDisabled(int index);\n    void SwapTexture(int index);\n", ""),
    ("inline bool CCompartment::SwapCollisionDisabled(int index)", "inline bool CCompartment::TextureSwapCollisionDisabled(int index)"),
    ("inline void CCompartment::SwapTexture(int index)", "inline void CCompartment::TextureSwap(int index)"),
    ("if (!SwapCollisionDisabled(indices.m_data[j]))\n                SwapTexture(", "if (!TextureSwapCollisionDisabled(indices.m_data[j]))\n                TextureSwap(")])
var("german_nodead", "objcreate_german", "src/game/objcreate_german.cpp", [
    ("    } else {\n        if (weapon) {\n            if (!(((WeaponView*)weapon)->m_anim->m_flags & 1))\n                DetachObject(\"lt_hand\");\n            else\n                DetachObject(\"rt_hand\");\n        }\n        m_currentWeapon = 0;", "    } else {\n        m_currentWeapon = 0;")])
var("oa_noisclear_dup", "ObstacleAvoidance", "src/game/ObstacleAvoidance.cpp", [
    ("distance < m_grid[line].m_distance[1] && distance < m_grid[line].m_distance[1] &&", "distance < m_grid[line].m_distance[1] &&")])
var("oa_merge_arms", "ObstacleAvoidance", "src/game/ObstacleAvoidance.cpp", [
    ("        case 7:\n            return false;\n        case 2:", "        case 7:\n        case 2:")])
var("oa_noswitch", "ObstacleAvoidance", "src/game/ObstacleAvoidance.cpp", [
    ("        switch (mask) {\n        case 0:\n        case 1:\n        case 5:\n        case 7:\n            return false;\n        case 2:\n        case 3:\n        case 6:\n            return false;\n        }\n", "        if (mask != 4)\n            return false;\n")])
var("dbg_novoid", "debug_prof_register", "src/game/debug_prof_register.cpp", [("    (void)0;\n", "")])
var("sku_realname", "ShellMenu_sku", "src/game/ShellMenu_sku.cpp", [
    ("    int CountWindows() {", "    int Get_NumPlayers() {"), ("int count = CountWindows();", "int count = Get_NumPlayers();")])
