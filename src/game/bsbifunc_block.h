// The head of bsbifunc.cpp's constant block (...rodata.0, 0x8016e820).
//
// BSInitBIFuncs reaches its messages and file names as block base + immediate
// offset (+648..+972), behind a table and strings placed by other functions of
// the file, some of them stripped by the original linker or not part of this
// project's fragments. This header copies those items from the original block.
// BsbifuncBlockStrings is a layout device, not recovered source: it has no
// logic, and each unit discards it (discarded_functions), so it never reaches
// the image. See docs/Dolphin.md, pool_references, and docs/Remaining.md
// (Source forms).
#ifndef BSBIFUNC_BLOCK_H
#define BSBIFUNC_BLOCK_H

struct BsbifuncBlockRecord {
    int v[10];
};

namespace dwi {
static const unsigned long dwi_prime_list[27] = {53u,97u,193u,389u,769u,1543u,3079u,6151u,12289u,24593u,49157u,98317u,196613u,393241u,786433u,1572869u,3145739u,6291469u,12582917u,25165843u,50331653u,100663319u,201326611u,402653189u,805306457u,1610612741u,3221225473u};
}

static const BsbifuncBlockRecord s_record = {{7, 3, 0, 0x1000, 0, 0, 0x9c4, 0, 0, 0x7fff}};

#pragma exceptions off
void BsbifuncBlockItem(const void*);
void BsbifuncBlockStrings() {
    BsbifuncBlockItem("-------------- Duplicate Creation:  Ignoring for total enemy count\n");
    BsbifuncBlockItem("%s%s's %s");
    BsbifuncBlockItem("Prop %d has already been attached or destroyed. Check with Designers for possible duplicate ID.\n");
    BsbifuncBlockItem("******WAYPOINTINFO_TARGET_ANGLE_TO_COVER *****\n");
    BsbifuncBlockItem("CoverInvAngle: %f, CoverToTargAngle: %f, AngleDiff: %f\n");
    BsbifuncBlockItem("\nbreak point!");
    BsbifuncBlockItem("Warning: Hearing Radius is zero, changing to default\n");
    BsbifuncBlockItem("WARNING: Your engine has no implementation for BIF CRC 0x%x\n...ignoring\n");
    BsbifuncBlockItem("  look in source\\ai_script\\bsbifunc.h for string name of BIF CRC\n");
}
#pragma exceptions reset

#endif
