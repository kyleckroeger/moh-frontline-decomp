// The head of pumhandler.cpp's constant block (...rodata.0, 0x80177738).
//
// The PopUpMessageHandler constructor reaches its message and sprite names as
// block base + immediate offset (+300..+408), behind two quad tables whose
// users the original linker removed and strings that other fragments of the
// file place themselves. This header copies those items from the original
// block. PumhandlerBlockStrings is a layout device, not recovered source: it
// has no logic, and each unit discards it (discarded_functions), so it never
// reaches the image. See docs/Dolphin.md, pool_references, and
// docs/Remaining.md (Source forms).
#ifndef PUMHANDLER_BLOCK_H
#define PUMHANDLER_BLOCK_H

struct PumBlockQuadVert {
    float u;
    float v;
    float x;
    float y;
    float z;
    unsigned long color;
};
struct PumBlockQuad {
    PumBlockQuadVert v[4];
};

namespace dwi {
static const unsigned long dwi_prime_list[27] = {53u,97u,193u,389u,769u,1543u,3079u,6151u,12289u,24593u,49157u,98317u,196613u,393241u,786433u,1572869u,3145739u,6291469u,12582917u,25165843u,50331653u,100663319u,201326611u,402653189u,805306457u,1610612741u,3221225473u};
}

static const PumBlockQuad s_quad0 = {{
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808040},
    {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808040},
    {1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x80808040},
    {0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x80808040}
}};
static const PumBlockQuad s_quad1 = {{
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808040},
    {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808040},
    {1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x80808040},
    {0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x80808040}
}};

#pragma exceptions off
void PumhandlerBlockItem(const void*);
void PumhandlerBlockStrings() {
    PumhandlerBlockItem("Can't find User message files: %s and %s!!!\n");
    PumhandlerBlockItem("Subtitle Queue");
    PumhandlerBlockItem("Subtitle queue");
}
#pragma exceptions reset

#endif
