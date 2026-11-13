// The head of rcmp_main.cpp's constant block (...rodata.0, 0x8017aac0).
//
// OPTION_PARSING_PLAYER::Play reaches its strings as block base + immediate
// offset (+32..+120), behind two strings whose users the original linker
// removed, so no surviving function can recreate their place. This header
// copies those two strings from the original block. RcmpMainBlockStrings is a
// layout device, not recovered source: it has no logic, and each unit discards
// it (discarded_functions), so it never reaches the image. See docs/Dolphin.md,
// pool_references, and docs/Remaining.md (Source forms).
#ifndef RCMP_MAIN_BLOCK_H
#define RCMP_MAIN_BLOCK_H

#pragma exceptions off
void RcmpMainBlockItem(const void*);
void RcmpMainBlockStrings() {
    RcmpMainBlockItem("Reseting.");
    RcmpMainBlockItem("Software Reseting.");
}
#pragma exceptions reset

#endif
