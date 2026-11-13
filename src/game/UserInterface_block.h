// The head of UserInterface.cpp's constant block (...rodata.0, 0x801778f8).
//
// UserInterface::Init reaches its path template and sprite names as block
// base + immediate offset (+108, +476..+664), behind tables and strings whose
// users the original linker removed (the multiplayer score lines, the
// controller messages) or that other fragments of the file place themselves.
// This header copies those items from the original block. UserInterfaceBlockStrings
// is a layout device, not recovered source: it has no logic, and each unit
// discards it (discarded_functions), so it never reaches the image. See
// docs/Dolphin.md, pool_references, and docs/Remaining.md (Source forms).
//
// LoadHeadshots is real code (Init's headshot loop). Its `char path[20] = ""`
// template is the block's second item (+108, @707, before the quad @875):
// CW 1.3.2 creates an inline function's local-array template once, where the
// inline is DEFINED, and every expansion copies from it (a function's own
// local makes a fresh template at its first use, after Init's names). So the
// inline is defined here, after the prime list. The include must come after
// the CSprite and UserInterface declarations it uses.
#ifndef USERINTERFACE_BLOCK_H
#define USERINTERFACE_BLOCK_H

struct UIBQuadVert {
    float u;
    float v;
    float x;
    float y;
    float z;
    unsigned long color;
};
struct UIBQuad {
    UIBQuadVert v[4];
};
struct UIBRecord {
    int v[10];
};

namespace dwi {
static const unsigned long dwi_prime_list[27] = {53u,97u,193u,389u,769u,1543u,3079u,6151u,12289u,24593u,49157u,98317u,196613u,393241u,786433u,1572869u,3145739u,6291469u,12582917u,25165843u,50331653u,100663319u,201326611u,402653189u,805306457u,1610612741u,3221225473u};
}

// Inferred name. The loop counter is the caller's (by reference): with its own
// counter the inlined loop colours the counter above the pointer it walks.
static inline void LoadHeadshots(int& i) {
    char path[20] = "";
    for (i = 0; i < 20; i++) {
        if (!UserInterface::headshotImages[i]) {
            sprintf(path, "bitmaps\\Model%02d.gsh", i + 1);
            UserInterface::headshotImages[i] = new CSprite(path, (CSprite::ESpriteLoadType)1, (ERenderPriority)0x6f);
        }
    }
}

static const UIBQuad s_quad = {{
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x808080FF},
    {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x808080FF},
    {1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x808080FF},
    {0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x808080FF}
}};
static const UIBRecord s_record0 = {{7, 3, 0, 0x1000, 0, 0, 0x9c4, 0, 0, 0}};
static const UIBRecord s_record1 = {{7, 3, 0, 0x1000, 0, 0, 0x9c4, 0, 0, 0}};

#pragma exceptions off
void UserInterfaceBlockItem(const void*);
void UserInterfaceBlockStrings() {
    UserInterfaceBlockItem("Team Score: %d");
    UserInterfaceBlockItem("Kills: %d");
    UserInterfaceBlockItem("Deaths: %d");
    UserInterfaceBlockItem("Preferred Target: %s");
    UserInterfaceBlockItem("Last Shot By: %s");
    UserInterfaceBlockItem("Accuracy: %d%%");
    UserInterfaceBlockItem("%d:%02d remaining");
    UserInterfaceBlockItem("Please reconnect the Controller\n");
    UserInterfaceBlockItem(" to Socket %d\n");
}
#pragma exceptions reset

#endif
