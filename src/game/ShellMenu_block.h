// The layout of ShellMenu.cpp's constant block (...rodata.0, 0x801783c8).
//
// The functions of this file reach their strings and tables as block base +
// immediate offset, so their code depends on where every item of the block
// sits, including items whose users the original linker removed. This header
// recreates that layout. Its strings and tables are copied from the original
// block. ShellMenuBlockStrings is a layout device, not recovered source: it has
// no logic, and each unit discards it (discarded_functions), so it never reaches
// the image. See docs/Dolphin.md, pool_references.
#ifndef SHELLMENU_BLOCK_H
#define SHELLMENU_BLOCK_H

struct _spritepolyvert {
    float u;
    float v;
    float x;
    float y;
    float z;
    unsigned long color;
};
struct SMBQuad {
    _spritepolyvert v[4];
};
struct SMBCreditTimes {
    int v[20];
};
struct SMBBriefingCounts {
    int v[6];
};
struct SMBBriefingTimes {
    int v[6][11];
};

namespace dwi {
static const unsigned long dwi_prime_list[27] = {53u,97u,193u,389u,769u,1543u,3079u,6151u,12289u,24593u,49157u,98317u,196613u,393241u,786433u,1572869u,3145739u,6291469u,12582917u,25165843u,50331653u,100663319u,201326611u,402653189u,805306457u,1610612741u,3221225473u};
}

static const char s_debugIni[] = "debug_REMOVE_FROM_FINAL.INI";
static const SMBCreditTimes s_creditTimes = {{250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250}};
static const SMBQuad s_creditQuad = {{
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080},
    {1.0f, 0.0f, 640.0f, 0.0f, 0.0f, 0x80808080},
    {1.0f, 1.0f, 640.0f, 448.0f, 0.0f, 0x80808080},
    {0.0f, 1.0f, 0.0f, 448.0f, 0.0f, 0x80808080}
}};
static const SMBBriefingCounts s_briefingCounts = {{11, 9, 10, 8, 6, 11}};
static const SMBBriefingTimes s_briefingTimes = {{
    {250, 410, 600, 520, 880, 820, 720, 380, 420, 540, 5000},
    {400, 600, 1500, 840, 1260, 860, 340, 480, 5000, 1, 1},
    {360, 840, 880, 920, 300, 800, 600, 900, 800, 5000, 1},
    {400, 500, 500, 300, 600, 700, 600, 5000, 1, 1, 1},
    {320, 800, 740, 940, 640, 5000, 1, 1, 1, 1, 1},
    {850, 850, 850, 800, 800, 800, 800, 800, 5000, 1, 1}
}};
static const SMBQuad s_briefingQuad = {{
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080},
    {1.0f, 0.0f, 640.0f, 0.0f, 0.0f, 0x80808080},
    {1.0f, 1.0f, 640.0f, 448.0f, 0.0f, 0x80808080},
    {0.0f, 1.0f, 0.0f, 448.0f, 0.0f, 0x80808080}
}};
static const SMBQuad s_unusedQuad = {{
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0xFFFFFFFF},
    {1.0f, 0.0f, 640.0f, 0.0f, 0.0f, 0xFFFFFFFF},
    {1.0f, 1.0f, 640.0f, 448.0f, 0.0f, 0xFFFFFFFF},
    {0.0f, 1.0f, 0.0f, 448.0f, 0.0f, 0xFFFFFFFF}
}};

#pragma exceptions off
void ShellMenuBlockItem(const void*);
void ShellMenuBlockStrings() {
    ShellMenuBlockItem("Formatting the Memory Card in Slot %c");
    ShellMenuBlockItem("will erase all previously saved data.");
    ShellMenuBlockItem("data\134objFont.gfn");
    ShellMenuBlockItem("The display mode has switched");
    ShellMenuBlockItem("to Progressive Mode.");
    ShellMenuBlockItem("to Interlaced Mode.");
    ShellMenuBlockItem("Press the A Button to continue.");
    ShellMenuBlockItem("This game can be played in Progressive Mode.");
    ShellMenuBlockItem("Press the A Button for Progressive Mode.");
    ShellMenuBlockItem("Press the B Button for Interlaced Mode.");
    ShellMenuBlockItem("Checkin Controller to see if B is pressed for progressive mode.\012");
    ShellMenuBlockItem("B Button IS DOWN!\012");
    ShellMenuBlockItem("Insufficient free files to save a new file on the");
    ShellMenuBlockItem("Insufficient free blocks to save a new file on the");
    ShellMenuBlockItem("Memory Card in Slot %c. ");
    ShellMenuBlockItem("This game requires at least 2 files and 4 blocks to");
    ShellMenuBlockItem("save, 1 file and 2 blocks are needed for the Preference");
    ShellMenuBlockItem("file and 1 file and 2 blocks for each Save Game file.");
    ShellMenuBlockItem("Existing files may be loaded and overwritten.");
    ShellMenuBlockItem("Press the B Button to manage the Memory Card");
    ShellMenuBlockItem("or press the A Button to continue.");
    ShellMenuBlockItem("Stiel Gran");
    ShellMenuBlockItem("S. Granate");
    ShellMenuBlockItem("Silenced");
    ShellMenuBlockItem("S. Pistol");
    ShellMenuBlockItem("Panzerschr");
    ShellMenuBlockItem("Gewehr 43");
    ShellMenuBlockItem("Springfiel");
    ShellMenuBlockItem("Pistola co");
    ShellMenuBlockItem("Pistola S.");
    ShellMenuBlockItem("Pistolet s");
    ShellMenuBlockItem("Pistolet S.");
    ShellMenuBlockItem("Grenade pr");
    ShellMenuBlockItem("Grenade PP");
    ShellMenuBlockItem("Fusil \340 po");
    ShellMenuBlockItem("Fusil Spri");
    ShellMenuBlockItem("Silencieux");
    ShellMenuBlockItem("PLAYER %d");
    ShellMenuBlockItem("data/start.gsh");
    ShellMenuBlockItem("data/victory.gsh");
    ShellMenuBlockItem("data/return.gsh");
    ShellMenuBlockItem("data/vict%d_%d.gsh");
    ShellMenuBlockItem("Please connect a controller to socket %d\012");
    ShellMenuBlockItem("No Controller Inserted\012");
    ShellMenuBlockItem("Level ID: %d\012");
    ShellMenuBlockItem("data\134shell\134brief%d1.asf");
    ShellMenuBlockItem("slide%d%d.gsh");
    ShellMenuBlockItem("data\134shell\134credits.asf");
    ShellMenuBlockItem("credits%d.gsh");
    ShellMenuBlockItem("data\134Movies\134extras%d.mpc");
    ShellMenuBlockItem("data\134Movies\134history%d.mpc");
    ShellMenuBlockItem("data\134Movies\134making%d.mpc");
    ShellMenuBlockItem("No Memory Card detected.");
    ShellMenuBlockItem("Please insert a Memory Card or press the");
    ShellMenuBlockItem("A Button to continue without saving.");
    ShellMenuBlockItem("m_MemoryCard->LoadPrefs() return false\012");
    ShellMenuBlockItem("Frontline");
    ShellMenuBlockItem("Prefs file is corrupted since I can't find the frontline signature\012");
    ShellMenuBlockItem("Memory Card Prefs point to slot %d for auto-load\012");
    ShellMenuBlockItem("Memory Card Prefs auto-load game is not present!\012");
    ShellMenuBlockItem("CRC check on load game failed: %x\012");
    ShellMenuBlockItem("Detected Erase Error!\012");
    ShellMenuBlockItem("Detected Program Error!\012");
    ShellMenuBlockItem("Current hit stats for Weapon[ %d ] = %d \012");
    ShellMenuBlockItem("It's been 30 seconds, play the movie.\012");
    ShellMenuBlockItem("shell menu");
    ShellMenuBlockItem("filename");
    ShellMenuBlockItem("bPromptsOn");
    ShellMenuBlockItem("data\134shell\134Shell1.asf");
    ShellMenuBlockItem("data\134chars.bum");
    ShellMenuBlockItem("data\134binum.bum");
    ShellMenuBlockItem("unplugged.gsh");
    ShellMenuBlockItem("data/objFont.gfn");
}
#pragma exceptions reset

#endif
