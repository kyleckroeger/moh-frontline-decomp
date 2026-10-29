// MemoryCardGC.cpp (0x800ee7a4): CMemoryCard, the game's memory card layer
// over the SDK's CARD library: probing and mounting a slot, the preference
// file ("MOHFLprefs": load, save, create, delete) and the numbered save games
// ("MOHFLxNNN": create with the C8 icon and comment header, iterate, open,
// delete), format, check and free-space queries. The functions are written
// in reverse image order: the file is compiled with deferred inlining, which
// emits them in reverse and lets Close and QuerySlot inline into the
// functions before them.
// CMemoryCard and the functions are named by the mangled symbols, the
// codename globals, icon data, header blocks and card buffer by their
// symbols; the members, the SDK record views (CARDFileInfo, CARDStat) and
// the return types are inferred (members at their offsets, names not
// original), the CARDSet* macros are the SDK's. The card functions take the
// SDK's s32 (long), which is why the slot is copied for those calls.
extern "C" {
void* memcpy(void*, const void*, unsigned long);
void* memset(void*, int, unsigned long);
char* strncpy(char*, const char*, unsigned long);
unsigned long strlen(const char*);
char* strstr(const char*, const char*);
int strncmp(const char*, const char*, unsigned long);
int sprintf(char*, const char*, ...);
int atoi(const char*);
}

/* inferred views of the SDK's CARDFileInfo and CARDStat */
struct CARDFileInfo {
    long chan;
    long fileNo;
    long offset;
    long length;
    unsigned short iBlock;
    unsigned short unknown12;
};

struct CARDStat {
    char fileName[32];
    unsigned long length;
    unsigned long time;
    unsigned char gameName[4];
    unsigned char company[2];
    unsigned char bannerFormat;
    unsigned char unknown2f;
    unsigned long iconAddr;
    unsigned short iconFormat;
    unsigned short iconSpeed;
    unsigned long commentAddr;
    unsigned char unknown3c[48];
};

#define CARD_STAT_ICON_MASK 3
#define CARD_STAT_BANNER_MASK 3
#define CARD_STAT_ANIM_MASK 4
#define CARD_STAT_SPEED_MASK 3
#define CARDSetBannerFormat(stat, f) ((stat)->bannerFormat = (unsigned char)(((stat)->bannerFormat & ~CARD_STAT_BANNER_MASK) | (f)))
#define CARDSetIconAnim(stat, f) ((stat)->bannerFormat = (unsigned char)(((stat)->bannerFormat & ~CARD_STAT_ANIM_MASK) | (f)))
#define CARDSetIconAddr(stat, addr) ((stat)->iconAddr = (unsigned long)(addr))
#define CARDSetIconFormat(stat, n, f) ((stat)->iconFormat = (unsigned short)(((stat)->iconFormat & ~(CARD_STAT_ICON_MASK << (2 * (n)))) | ((f) << (2 * (n)))))
#define CARDSetIconSpeed(stat, n, f) ((stat)->iconSpeed = (unsigned short)(((stat)->iconSpeed & ~(CARD_STAT_SPEED_MASK << (2 * (n)))) | ((f) << (2 * (n)))))
#define CARDSetCommentAddr(stat, addr) ((stat)->commentAddr = (unsigned long)(addr))

extern "C" {
void CARDInit();
long CARDProbe(long);
long CARDProbeEx(long, long*, long*);
long CARDMountAsync(long, void*, void*, void*);
long CARDUnmount(long);
long CARDCheckAsync(long, void*);
long CARDFormatAsync(long, void*);
long CARDGetResultCode(long);
long CARDFreeBlocks(long, long*, long*);
long CARDGetSectorSize(long, unsigned long*);
long CARDOpen(long, char*, CARDFileInfo*);
long CARDFastOpen(long, long, CARDFileInfo*);
long CARDClose(CARDFileInfo*);
long CARDCreateAsync(long, char*, unsigned long, CARDFileInfo*, void (*)(long, long));
long CARDDelete(long, char*);
long CARDReadAsync(CARDFileInfo*, void*, long, long, void (*)(long, long));
long CARDWriteAsync(CARDFileInfo*, void*, long, long, void (*)(long, long));
long CARDWrite(CARDFileInfo*, void*, long, long);
long CARDGetStatus(long, long, CARDStat*);
long CARDSetStatus(long, long, CARDStat*);
long CARDSetAttributes(long, long, unsigned char);
}

void DebugMsg(const char*, ...);
void MohDVDErrorTask(int, int);

char* GAME_CODENAME = "GMFE";
char* COMPANY_CODENAME = "69";
char* COMPANY_DEBUGNAME = "CD";
char* GAME_DEBUGNAME = "DEBG";

static bool SysCardInit;

static const unsigned short ICON_CLUT_DATA[256] = {
    0x8000, 0xadce, 0xd2f6, 0x8ca4, 0xe779, 0xba51, 0x9d29, 0x8440, 0xf3bb, 0xdf37, 0xc6b4, 0xa56c,
    0xb210, 0x98c8, 0x8863, 0xeb9a, 0xe337, 0xc651, 0x9d2a, 0x94c5, 0xc22d, 0xadf0, 0x8020, 0xdaf4,
    0x8464, 0xd2b3, 0xbe73, 0xf7bb, 0xe339, 0xb231, 0xef9a, 0x8042, 0xa9ac, 0x9d08, 0xc673, 0xa546,
    0x8cc6, 0xdaf7, 0xa16a, 0x9d05, 0xb5ed, 0xe77a, 0xb5f0, 0x9062, 0xceb4, 0xe759, 0x8c84, 0xc251,
    0xadac, 0x8cc6, 0xdb38, 0xd2f7, 0x8841, 0xa569, 0xeb7a, 0xb631, 0xadf1, 0xc6d6, 0xc675, 0xef9b,
    0xbe30, 0x9ca7, 0x8000, 0xf7dc, 0xce70, 0x8021, 0xeb9b, 0xe758, 0xa56b, 0xb630, 0x9908, 0x9d2a,
    0xefbc, 0x8ca3, 0xced5, 0x8c43, 0x8ca6, 0xa9ad, 0x8441, 0xb5ef, 0xe359, 0xa58a, 0x94c7, 0x9d49,
    0xeb7a, 0xc273, 0xe315, 0xd718, 0x9506, 0xd738, 0xb612, 0xc694, 0xa148, 0xc293, 0xa52b, 0xcab4,
    0x8820, 0x90e7, 0xc651, 0xadae, 0xb20f, 0xbe73, 0x9ce6, 0xdb18, 0xd6f5, 0xf39b, 0xa9ae, 0x8442,
    0xb1ee, 0xe338, 0x8466, 0xf7bc, 0xd691, 0x9d06, 0xbdee, 0xb212, 0xd6f7, 0xba0f, 0xdef5, 0x8000,
    0xd2d5, 0xdf38, 0x90a5, 0x9ce9, 0xfbdc, 0xdf38, 0xef9b, 0x98c4, 0xa129, 0xdb17, 0xe759, 0x8442,
    0x94c6, 0xad8d, 0xb632, 0xe359, 0xba52, 0xa96c, 0xa129, 0xa98b, 0xb1ac, 0xef9a, 0xe77a, 0x9082,
    0xb1f0, 0xb5ee, 0xeb79, 0xc273, 0xb630, 0x90a4, 0x8000, 0x8420, 0x8c42, 0xa56b, 0xc272, 0xd6f6,
    0xb1ee, 0xd6d3, 0xf39a, 0xca74, 0x8c84, 0xced5, 0xef7a, 0x8421, 0x90e5, 0xeb7a, 0xe359, 0xdf38,
    0xd2d5, 0x9d0a, 0xcab4, 0xe358, 0x8463, 0xfbdc, 0xba33, 0x8400, 0xf7bc, 0xc694, 0xf3bb, 0xbe30,
    0xa18a, 0x9d07, 0xa54b, 0x98e5, 0x8c66, 0xeb79, 0xf39b, 0xdf39, 0x8885, 0x98e8, 0xba31, 0xdaf5,
    0xd6f7, 0xc692, 0xe759, 0x98e6, 0xa96a, 0xa58b, 0xbe94, 0x9484, 0x9065, 0xef9b, 0xadcc, 0xdf16,
    0xa128, 0xd2b5, 0xce71, 0xcad5, 0xa928, 0x9d09, 0xb60f, 0x8421, 0xf7bc, 0xadac, 0x9d49, 0xc6b5,
    0x8000, 0xe359, 0x9107, 0xadcd, 0xb211, 0xb5ee, 0xf7bc, 0xdb38, 0xb60f, 0x8ca6, 0x90c6, 0xdf39,
    0xfbdd, 0xeb7b, 0xc672, 0xd6f6, 0xced6, 0xdf16, 0x8443, 0x8861, 0xa10a, 0xe77a, 0xeb79, 0xa56d,
    0xcad6, 0x8840, 0xeb9a, 0xf7bb, 0x9cc5, 0xeb7a, 0xe359, 0xb631, 0x8020, 0xc693, 0xba51, 0xb1cf,
    0xbe72, 0xef9a, 0xf3bc, 0x8c86,
};

static const unsigned char ICON_TEXTURE_DATA[1024] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x1c, 0x25, 0xd9, 0x87, 0xbb, 0x82, 0xd9,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0x29, 0x29, 0x54, 0xc2, 0x29, 0xc2, 0x79,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x81, 0x9b, 0x4a, 0xa1, 0xaa, 0xaa, 0xaa,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xaa, 0xaa, 0xa1, 0x4a, 0xa8, 0xa8, 0xa8,
    0xd9, 0x87, 0xd9, 0xd9, 0x50, 0x2a, 0x1c, 0xbb, 0xf5, 0xf5, 0xe5, 0xed, 0xf5, 0xf5, 0xf5, 0xed,
    0xe3, 0xbb, 0x20, 0x64, 0xc0, 0xbb, 0x67, 0x57, 0xf5, 0x42, 0xf5, 0xe3, 0xdf, 0xe3, 0xbb, 0xbb,
    0x2a, 0x6d, 0x1c, 0x79, 0x1c, 0x1c, 0x87, 0xc2, 0xb3, 0x12, 0x0b, 0xa7, 0xa6, 0xc2, 0x0f, 0x54,
    0x6e, 0x1f, 0x1f, 0x81, 0x7d, 0xdf, 0x36, 0xcd, 0x77, 0x4c, 0x4e, 0x22, 0xdf, 0xdf, 0xdf, 0xe3,
    0x2a, 0x04, 0xc2, 0x82, 0x1c, 0x2a, 0x25, 0x9b, 0xb9, 0x2d, 0xab, 0xab, 0xa6, 0xb9, 0xb9, 0xa7,
    0xf2, 0x8d, 0x0f, 0xfd, 0x6f, 0x2a, 0x9f, 0xb2, 0x50, 0xd9, 0x29, 0xa2, 0xfd, 0xf2, 0x0f, 0xfd,
    0xc0, 0x74, 0x9b, 0x74, 0x02, 0x02, 0x74, 0xa8, 0xa7, 0xc0, 0x81, 0xc2, 0xfd, 0xc9, 0xc9, 0xba,
    0x69, 0x1e, 0x2d, 0x82, 0xa5, 0xf5, 0xf5, 0xa5, 0x3b, 0x36, 0xfd, 0x3b, 0xba, 0x3b, 0x7e, 0x42,
    0x42, 0x42, 0x7e, 0x7e, 0xed, 0xd9, 0xf6, 0xf6, 0xf2, 0x42, 0x42, 0x42, 0x7e, 0x7e, 0xc9, 0x7e,
    0xed, 0xed, 0xfe, 0xfe, 0xd4, 0xd4, 0x3f, 0xd4, 0xbb, 0x32, 0xa1, 0x4f, 0x2a, 0x33, 0x32, 0x04,
    0x00, 0x00, 0x80, 0x57, 0xbb, 0xbb, 0xbb, 0xbb, 0x25, 0x3d, 0xc3, 0x40, 0xfe, 0x7e, 0xc9, 0x48,
    0x8c, 0x52, 0x0b, 0x99, 0x5c, 0xad, 0x7c, 0xad, 0x2e, 0xef, 0x21, 0x8a, 0x62, 0x11, 0x57, 0xa6,
    0x50, 0xf6, 0xd9, 0xd9, 0x82, 0x29, 0xed, 0x54, 0x7e, 0x42, 0x42, 0x7e, 0xc9, 0xf5, 0x42, 0xfe,
    0x7c, 0x7c, 0x7c, 0x7d, 0x7c, 0x7c, 0x7c, 0xe4, 0x69, 0xb0, 0x69, 0x69, 0xb0, 0xb0, 0xb0, 0xad,
    0xf5, 0xf5, 0x8e, 0x29, 0x8e, 0x29, 0xf6, 0x59, 0x7e, 0xc9, 0x7e, 0x42, 0x7e, 0xf5, 0xed, 0x8e,
    0x7c, 0xad, 0xe4, 0xe4, 0x3f, 0x3f, 0xe4, 0x3f, 0xad, 0x3f, 0x3f, 0xad, 0x3f, 0xad, 0xad, 0xb0,
    0x9e, 0x9e, 0x1e, 0x8d, 0x5b, 0xbe, 0x9e, 0x69, 0x9e, 0x9e, 0x2d, 0x9e, 0x9e, 0x69, 0x02, 0xd1,
    0xf3, 0x9e, 0xab, 0x08, 0x08, 0x8d, 0x85, 0x4c, 0xb0, 0xb6, 0x5e, 0x89, 0xec, 0xb6, 0xfb, 0xbc,
    0xe1, 0xcc, 0x99, 0x71, 0xd5, 0xdd, 0xde, 0xde, 0xe1, 0x7a, 0x61, 0x91, 0x62, 0xcc, 0x0a, 0xde,
    0x2e, 0x60, 0x6b, 0x4f, 0x30, 0x21, 0xc4, 0x3b, 0xea, 0xaf, 0x8a, 0xeb, 0x00, 0x4f, 0x46, 0x3a,
    0xde, 0xde, 0xde, 0xf0, 0x08, 0xb0, 0xb0, 0x1c, 0xde, 0xde, 0xad, 0x29, 0xb0, 0xb0, 0xde, 0xde,
    0xde, 0xb0, 0x90, 0xfb, 0x63, 0x88, 0x5a, 0x86, 0x3a, 0x88, 0x45, 0x90, 0xdc, 0x1d, 0xae, 0xae,
    0xf3, 0xb9, 0xdf, 0x36, 0x6f, 0xb2, 0x33, 0x39, 0xde, 0xf3, 0xbb, 0xd7, 0xc6, 0xf3, 0xe3, 0x1c,
    0x1d, 0x5a, 0x73, 0xc6, 0xc6, 0x38, 0x1d, 0x38, 0xae, 0x5a, 0x6a, 0x36, 0x8d, 0x92, 0xa2, 0x1e,
    0x1b, 0xd1, 0xec, 0x7b, 0xb6, 0x86, 0xec, 0xbc, 0x92, 0xa9, 0xbd, 0x0d, 0xec, 0xec, 0x5e, 0xea,
    0xc1, 0x52, 0xbd, 0x43, 0xee, 0x36, 0xc4, 0x4e, 0xdd, 0xff, 0xff, 0x03, 0x92, 0x43, 0x18, 0x1f,
    0xd3, 0x3e, 0xd3, 0x00, 0x00, 0xcc, 0x3c, 0x91, 0x4e, 0x00, 0xd8, 0xf8, 0x0e, 0x8b, 0x13, 0x72,
    0xbc, 0x00, 0x00, 0x00, 0x77, 0x00, 0x77, 0x11, 0x3e, 0x41, 0x41, 0x00, 0x00, 0xb5, 0x95, 0xc4,
    0xb2, 0xf3, 0xba, 0x36, 0x81, 0x69, 0x82, 0xa6, 0x04, 0x1e, 0xb2, 0xd4, 0xde, 0xde, 0xde, 0x6f,
    0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0xb2, 0x69, 0x6f, 0x2d, 0x55, 0x4a, 0x68, 0xab, 0x02, 0x10,
    0x54, 0x1e, 0xe8, 0x4a, 0x9e, 0x92, 0x8d, 0x09, 0x6f, 0xf3, 0xb2, 0x1e, 0x81, 0x09, 0xaa, 0x81,
    0xab, 0x10, 0x69, 0xaa, 0x10, 0x15, 0xbd, 0xb1, 0x43, 0xa8, 0x5b, 0x2f, 0xd7, 0x93, 0x5d, 0xd1,
    0xc4, 0xb8, 0x0d, 0x02, 0xc8, 0x68, 0x1f, 0xac, 0x91, 0xdb, 0x55, 0xcb, 0xe7, 0x9a, 0x18, 0x00,
    0x75, 0xa0, 0x61, 0x2c, 0x09, 0xcf, 0x41, 0x00, 0x35, 0x0e, 0x0a, 0xbf, 0x65, 0x5d, 0x46, 0x68,
    0x00, 0x00, 0xf8, 0xf8, 0x00, 0x00, 0x07, 0x71, 0x00, 0x34, 0x84, 0xea, 0x00, 0xeb, 0x49, 0xc3,
    0x6b, 0x2b, 0x00, 0x97, 0x00, 0xf8, 0xb5, 0xdd, 0xd8, 0x00, 0x00, 0x00, 0xd8, 0x00, 0xfa, 0x44,
    0xc0, 0x47, 0x9c, 0xa1, 0xa2, 0x10, 0x43, 0x10, 0xee, 0x4a, 0x94, 0x1a, 0x01, 0xe0, 0xb4, 0x6c,
    0x8c, 0x91, 0x01, 0x03, 0x58, 0xd2, 0xb4, 0x26, 0xf7, 0xe9, 0x03, 0x24, 0xd6, 0x24, 0x64, 0xa4,
    0x43, 0x9c, 0x2d, 0x3c, 0xd6, 0x01, 0xa4, 0x94, 0x5d, 0x75, 0xaa, 0x5b, 0xa4, 0x4d, 0xab, 0x6d,
    0xca, 0x53, 0x53, 0xc5, 0xe7, 0xe6, 0xb3, 0xdd, 0x51, 0x64, 0x5f, 0x88, 0x78, 0xbf, 0x02, 0x02,
    0x28, 0x65, 0x65, 0xfa, 0xf7, 0x01, 0x5d, 0xc5, 0x4d, 0x53, 0xda, 0x31, 0xd6, 0xdb, 0x06, 0xcc,
    0x4e, 0xa0, 0xe2, 0x84, 0x30, 0x30, 0x6b, 0x46, 0x2e, 0x7a, 0x0e, 0x58, 0x2e, 0xea, 0xa4, 0x26,
    0x84, 0x66, 0x00, 0x3e, 0x4b, 0x66, 0x35, 0x4d, 0xf4, 0x13, 0x3e, 0xa3, 0x4b, 0xd0, 0x4e, 0x61,
    0x2b, 0xc7, 0x00, 0x3e, 0xaf, 0x34, 0x19, 0xc1, 0xc7, 0x98, 0x96, 0x3e, 0x98, 0xf8, 0x9c, 0x75,
    0x20, 0x37, 0x20, 0xf9, 0xfa, 0x76, 0x76, 0x0c, 0x6b, 0x05, 0x9d, 0x6c, 0x19, 0x64, 0x45, 0x0c,
    0x9a, 0x9a, 0x0c, 0xd3, 0x83, 0x9c, 0x9a, 0xdb, 0x17, 0x76, 0x56, 0xce, 0x70, 0x14, 0x5c, 0x27,
    0x6c, 0x45, 0xfc, 0x45, 0x93, 0x2c, 0x5d, 0xc5, 0x46, 0xc5, 0x0c, 0x80, 0x06, 0xb7, 0xd3, 0x34,
    0xeb, 0x23, 0x7f, 0x8f, 0xf1, 0xf1, 0x97, 0xf1, 0x8f, 0x60, 0x60, 0x16, 0x97, 0xf1, 0x97, 0x97,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

class CMemoryCard {
public:
    CMemoryCard();
    ~CMemoryCard();
    void Initialize();
    bool QuerySlot(int);
    int OpenSlot(int);
    bool IsEncodingCorrect(int);
    bool IsAnyDeviceInPort(int);
    bool Is8KCard(int);
    int MountPortIfRequired(int);
    int GetMaxBlocksOnCard(int);
    int IsFormated();
    unsigned long GetAvailableMemory();
    long GetFilesFree(int);
    long GetCheckCardStatus();
    int GetNumSaveGames();
    bool CreateNewSaveGame(unsigned int*, char*);
    bool Format();
    void RewindSaveGameList();
    int GetNextSaveGame(unsigned int*, unsigned int*, char*);
    bool IsSaveGamePresent(int);
    bool DeleteSaveGame(unsigned int);
    bool DeletePrefFile(int);
    bool Open(unsigned int, bool);
    bool Close();
    bool Read(unsigned char*, unsigned int);
    bool Write(unsigned char*, unsigned int);
    bool CreatePrefs();
    bool SavePrefs(unsigned char*, unsigned int);
    bool LoadPrefs(unsigned char*, unsigned int);
    bool ArePrefsPresent();
    int GetSaveGameOverheadSize(bool);
    int GetPort();

    unsigned char unknown00000[4];
    int m_slot;
    CARDFileInfo m_fileInfo;
    unsigned long m_sectorSize;
    unsigned short m_saveGameIndex;
    unsigned short m_lastSaveGame;
    bool m_isSave[127];
    bool m_seen[127];
    bool m_mounted[2];
    unsigned char unknown00124[28];
    unsigned char m_workArea[81920];
    char m_name[32];
};

CMemoryCard::CMemoryCard() {
    if (!SysCardInit) {
        CARDInit();
        SysCardInit = true;
    }
    m_mounted[0] = false;
    m_mounted[1] = false;
    memset(m_name, 0, 32);
}

CMemoryCard::~CMemoryCard() {
}

void CMemoryCard::Initialize() {
}

bool CMemoryCard::QuerySlot(int slot) {
    int result;
    while ((result = CARDProbe(slot)) == -1)
        ;
    if (result == 0) {
        if (m_mounted[slot])
            CARDUnmount(slot);
        m_mounted[slot] = false;
        return 0;
    }
    for (;;) {
        while ((result = CARDProbeEx(slot, 0, 0)) == -1)
            ;
        if (result < 0) {
            if (m_mounted[slot])
                CARDUnmount(slot);
            m_mounted[slot] = false;
        }
        return 1;
    }
}

int CMemoryCard::OpenSlot(int slot) {
    int result;
    m_slot = slot;
    while ((result = CARDProbe(slot)) == -1)
        ;
    if (result == 0) {
        if (m_mounted[slot])
            CARDUnmount(slot);
        m_mounted[slot] = false;
        return 0;
    }
    for (;;) {
        while ((result = CARDProbeEx(slot, 0, 0)) == -1)
            ;
        if (result < 0) {
            if (m_mounted[slot])
                CARDUnmount(slot);
            m_mounted[slot] = false;
        }
        if (result == -2)
            return 0;
        return 1;
    }
}

bool CMemoryCard::IsEncodingCorrect(int slot) {
    if (m_mounted[slot]) {
        CARDUnmount(slot);
        m_mounted[slot] = false;
    }
    long result = CARDMountAsync(slot, m_workArea, 0, 0);
    if (result < 0) {
        DebugMsg("Mount failed. Error code %d returned\n", result);
        return true;
    }
    while ((result = CARDGetResultCode(slot)) == -1)
        ;
    CARDUnmount(slot);
    m_mounted[slot] = false;
    return result != -13;
}

bool CMemoryCard::IsAnyDeviceInPort(int slot) {
    long result;
    while ((result = CARDProbe(slot)) == -1)
        ;
    if (result == 0) {
        if (m_mounted[slot])
            CARDUnmount(slot);
        m_mounted[slot] = false;
        return false;
    }
    return true;
}

bool CMemoryCard::Is8KCard(int slot) {
    long memSize;
    long sectorSize;
    long result;
    while ((result = CARDProbeEx(slot, &memSize, &sectorSize)) == -1)
        ;
    if (result < 0) {
        if (m_mounted[slot])
            CARDUnmount(slot);
        m_mounted[slot] = false;
        return true;
    }
    if (sectorSize == 8192)
        return true;
    return false;
}

int CMemoryCard::MountPortIfRequired(int slot) {
    long result;
    if (!QuerySlot(slot))
        return 0;
    if (m_mounted[slot])
        return 1;
    result = CARDMountAsync(slot, m_workArea, 0, 0);
    if (result < 0) {
        if (result == -5 || result == -128)
            return 3;
        DebugMsg("Mount failed. Error code %d returned\n", result);
        return 0;
    }
    while ((result = CARDGetResultCode(slot)) == -1)
        ;
    switch (result) {
    case -128:
    case -5:
        DebugMsg("Card mount failure. Error code %d returned\n", result);
        return 3;
    case -13:
        DebugMsg("Card mount failure. Error code %d returned\n", result);
        CARDUnmount(slot);
        m_mounted[slot] = false;
        return 0;
    default:
        DebugMsg("Card mount failure. Error code %d returned\n", result);
        return 0;
    case 0:
    case -6:
        break;
    }
    CARDCheckAsync(slot, 0);
    while ((result = CARDGetResultCode(slot)) == -1)
        MohDVDErrorTask(0, 0);
    if (result != 0) {
        DebugMsg("CARDCheck failed. Error code %d returned\n", result);
        CARDUnmount(slot);
        m_mounted[slot] = false;
        if (result == -6)
            return 2;
        if (result == -5 || result == -128)
            return 3;
        return 0;
    }
    m_mounted[slot] = true;
    return 1;
}

int CMemoryCard::GetMaxBlocksOnCard(int slot) {
    long memSize = 0;
    long sectorSize = 8192;
    while (CARDProbeEx(slot, &memSize, &sectorSize) == -1)
        ;
    return (memSize << 17) / sectorSize - 5;
}

int CMemoryCard::IsFormated() {
    if (m_mounted[m_slot]) {
        CARDUnmount(m_slot);
        m_mounted[m_slot] = false;
    }
    DebugMsg("Attempting to mount a card into slot %d\n", m_slot);
    int result = MountPortIfRequired(m_slot);
    if (result != 1)
        return result;
    result = CARDGetSectorSize(m_slot, &m_sectorSize);
    if (result < 0) {
        DebugMsg("CARDGetSectorSize() failed on error code %d\n", result);
        return 0;
    }
    return 1;
}

unsigned long CMemoryCard::GetAvailableMemory() {
    unsigned long bytesNotUsed;
    long filesNotUsed;
    unsigned long sectorSize;
    long result;
    if (MountPortIfRequired(m_slot) != 1)
        return 0;
    result = CARDGetSectorSize(m_slot, &sectorSize);
    if (result < 0) {
        DebugMsg("CARDGetSectorSize() failed. (%d)\n", result);
        CARDUnmount(m_slot);
        m_mounted[m_slot] = false;
        return 0;
    }
    DebugMsg("Sector size %u bytes.\n", sectorSize);
    m_sectorSize = sectorSize;
    result = CARDFreeBlocks(m_slot, (long*)&bytesNotUsed, &filesNotUsed);
    if (result < 0) {
        DebugMsg("Free Blocks failed. (%d)\n", result);
        CARDUnmount(m_slot);
        m_mounted[m_slot] = false;
        return 0;
    }
    DebugMsg("%d bytes (%d blocks) free.\n%d files free.\n", bytesNotUsed, bytesNotUsed / sectorSize, filesNotUsed);
    return bytesNotUsed / sectorSize;
}

long CMemoryCard::GetFilesFree(int slot) {
    long bytesNotUsed;
    long filesNotUsed;
    long result;
    if (MountPortIfRequired(slot) != 1)
        return 0;
    result = CARDFreeBlocks(slot, &bytesNotUsed, &filesNotUsed);
    if (result < 0) {
        DebugMsg("Free Blocks failed. (%d)\n", result);
        CARDUnmount(slot);
        m_mounted[slot] = false;
        return 0;
    }
    return filesNotUsed;
}

long CMemoryCard::GetCheckCardStatus() {
    long result;
    long slot;
    CARDCheckAsync(m_slot, 0);
    slot = m_slot;
    while ((result = CARDGetResultCode(slot)) == -1)
        MohDVDErrorTask(0, 0);
    return result;
}

// GetNumSaveGames stays a call in RewindSaveGameList (deferred auto-inlining
// would otherwise pull it in there).
#pragma dont_inline on
int CMemoryCard::GetNumSaveGames() {
    int count;
    int i;
    count = 0;
    for (i = 0; i < 127; i++) {
        m_isSave[i] = false;
        m_seen[i] = false;
    }
    if (MountPortIfRequired(m_slot) != 1)
        return 0;
    for (i = 0; i < 127; i++) {
        CARDFileInfo info;
        CARDStat stat;
        if (CARDFastOpen(m_slot, i, &info) == 0) {
            if (CARDGetStatus(m_slot, info.fileNo, &stat) == 0 && strstr(stat.fileName, "MOHFL") &&
                (strncmp((char*)stat.gameName, GAME_CODENAME, 4) == 0 ||
                 strncmp((char*)stat.gameName, GAME_DEBUGNAME, 4) == 0) &&
                (strncmp((char*)stat.company, COMPANY_CODENAME, 2) == 0 ||
                 strncmp((char*)stat.company, COMPANY_DEBUGNAME, 2) == 0) &&
                !strstr(stat.fileName, "prefs")) {
                m_isSave[i] = true;
                count++;
            }
            CARDClose(&info);
        }
    }
    CARDUnmount(m_slot);
    m_mounted[m_slot] = false;
    return count;
}
#pragma dont_inline reset

bool CMemoryCard::CreateNewSaveGame(unsigned int* index, char* name) {
    static unsigned char CARDData[8192] __attribute__((aligned(32)));
    static char HeaderC8[50][32] = {"Medal Of Honor Frontline\n", "Save: \n"};
    unsigned long sectorSize;
    CARDFileInfo info;
    char fileName[36];
    CARDStat stat;
    long result;
    unsigned long slot;
    int i;
    if (MountPortIfRequired(m_slot) != 1)
        return false;
    for (i = 0; m_seen[i] && i < 127; i++)
        ;
    m_lastSaveGame = i;
    *index = m_lastSaveGame;
    sprintf(fileName, "MOHFLx%03d", *index);
    result = CARDGetSectorSize(m_slot, &sectorSize);
    if (result < 0) {
        DebugMsg("CARDGetSectorSize() failed on error code %d\n", result);
        return false;
    }
    DebugMsg("Sector size is %d bytes.\n", sectorSize);
    result = CARDCreateAsync(m_slot, fileName, sectorSize * 2, &info, 0);
    if (result < 0) {
        switch (result) {
        case -128:
        case -12:
        case -6:
        case -4:
        case -1:
            break;
        case -7:
            DebugMsg("Failed to create file because file already exists!\n");
            if (CARDDelete(m_slot, fileName) != 0) {
                DebugMsg("Failed to delete file\n");
                return false;
            }
            CARDCreateAsync(m_slot, fileName, sectorSize * 2, &info, 0);
            while ((result = CARDGetResultCode(m_slot)) == -1)
                ;
            break;
        case -2:
            /* a dead self-assignment that CW drops after building the case tree; it keeps
               -2 a separate case target, which gives the original's compare order */
            result = (int)result;
        }
        if (result < 0) {
            DebugMsg("Failed file create with error code %d\n", result);
            return false;
        }
    }
    slot = m_slot;
    while ((result = CARDGetResultCode(slot)) == -1)
        MohDVDErrorTask(0, 0);
    result = CARDGetStatus(m_slot, info.fileNo, &stat);
    if (result < 0) {
        DebugMsg("CARDGetStatus() failed on error code %d\n", result);
        return false;
    }
    CARDSetAttributes(m_slot, info.fileNo, 28);
    strncpy(HeaderC8[1] + strlen("Save: "), name, 10);
    CARDSetCommentAddr(&stat, 0);
    CARDSetIconAddr(&stat, 64);
    CARDSetBannerFormat(&stat, 0);
    CARDSetIconAnim(&stat, 0);
    CARDSetIconFormat(&stat, 0, 1);
    CARDSetIconSpeed(&stat, 0, 3);
    CARDSetIconSpeed(&stat, 1, 0);
    strncpy((char*)stat.company, COMPANY_CODENAME, 2);
    strncpy((char*)stat.gameName, GAME_CODENAME, 4);
    result = CARDSetStatus(m_slot, info.fileNo, &stat);
    if (result < 0) {
        DebugMsg("Failed to set CARDStat in CreateNewSaveGame\n");
        return false;
    }
    memcpy(HeaderC8[2], ICON_TEXTURE_DATA, 1024);
    memcpy(HeaderC8[34], ICON_CLUT_DATA, 512);
    memcpy(CARDData, HeaderC8, 1600);
    CARDWriteAsync(&info, CARDData, stat.length, 0, 0);
    slot = m_slot;
    while ((result = CARDGetResultCode(slot)) == -1)
        MohDVDErrorTask(0, 0);
    if (result < 0)
        DebugMsg("Failed to CARDWrite in CreateNewSaveGame.\n");
    DebugMsg("gameName %.4s\n", stat.gameName);
    DebugMsg("company %.2s\n", stat.company);
    DebugMsg("length %d\n", stat.length);
    DebugMsg("fileName %.32s\n", stat.fileName);
    DebugMsg("time %d\n", stat.time);
    return true;
}

bool CMemoryCard::Format() {
    long result;
    long slot;

    CARDMountAsync(m_slot, m_workArea, 0, 0);
    slot = m_slot;
    while ((result = CARDGetResultCode(slot)) == -1)
        MohDVDErrorTask(0, 0);
    result = CARDFormatAsync(m_slot, 0);
    if (result < 0) {
        DebugMsg("Format failed with error %d\n", result);
        CARDUnmount(m_slot);
        m_mounted[m_slot] = false;
        return false;
    }
    slot = m_slot;
    while ((result = CARDGetResultCode(slot)) == -1)
        MohDVDErrorTask(0, 0);
    if (result < 0) {
        DebugMsg("Format failed with error %d\n", result);
        CARDUnmount(m_slot);
        m_mounted[m_slot] = false;
        return false;
    }
    CARDUnmount(m_slot);
    m_mounted[m_slot] = false;
    return true;
}

void CMemoryCard::RewindSaveGameList() {
    m_saveGameIndex = 0;
    GetNumSaveGames();
}

int CMemoryCard::GetNextSaveGame(unsigned int* index, unsigned int* size, char* name) {
    char comment[32];
    CARDStat stat;
    unsigned char buffer[8192] __attribute__((aligned(32)));
    long result;
    if (MountPortIfRequired(m_slot) != 1)
        return 0;
    while (!m_isSave[m_saveGameIndex]) {
        if (m_saveGameIndex >= 126) {
            int i;
            for (i = 0; m_seen[i] && i < 127; i++)
                ;
            *index = i;
            m_lastSaveGame = i;
            return 0;
        }
        m_saveGameIndex++;
    }
    if (m_saveGameIndex >= 127) {
        *index = m_saveGameIndex;
        m_lastSaveGame = m_saveGameIndex;
        return 0;
    }
    if (CARDFastOpen(m_slot, m_saveGameIndex, &m_fileInfo) != 0)
        return 2;
    *size = m_sectorSize;
    result = CARDGetStatus(m_slot, m_fileInfo.fileNo, &stat);
    if (result < 0) {
        DebugMsg("CARDGetStatus() failed on error code %d\n", result);
        return 2;
    }
    result = CARDReadAsync(&m_fileInfo, buffer, m_sectorSize, 0, 0);
    if (result < 0) {
        DebugMsg("Data read failed with error code %d\n", result);
        return 2;
    }
    while ((result = CARDGetResultCode(m_slot)) == -1)
        ;
    if (result < 0) {
        DebugMsg("Data read failed with error code %d\n", result);
        return 2;
    }
    strncpy(comment, (char*)buffer + 32, 32);
    char* p = comment;
    bool corrupt = false;
    int length = 0;
    while (*p != 0 && *p != '\n') {
        if (length > strlen("Save: ") + 10) {
            corrupt = true;
            strncpy(m_name, comment, 32);
            sprintf(name, "corrupt");
            break;
        }
        p++;
        length++;
    }
    if (length == 0)
        return 2;
    strncpy(name, comment + strlen("Save: "), 10);
    name[10] = 0;
    *index = atoi(&stat.fileName[strlen("MOHFL") + 1]);
    if (*index > 127)
        return 2;
    m_seen[*index] = true;
    Close();
    m_saveGameIndex++;
    return corrupt ? 2 : 1;
}

bool CMemoryCard::IsSaveGamePresent(int index) {
    char fileName[36];
    if (MountPortIfRequired(m_slot) != 1)
        return false;
    sprintf(fileName, "MOHFLx%03d", index);
    if (CARDOpen(m_slot, fileName, &m_fileInfo) != 0)
        return false;
    Close();
    return true;
}

bool CMemoryCard::DeleteSaveGame(unsigned int index) {
    char fileName[36];
    if (MountPortIfRequired(m_slot) != 1)
        return false;
    sprintf(fileName, "MOHFLx%03d", index);
    long result = CARDDelete(m_slot, fileName);
    CARDUnmount(m_slot);
    m_mounted[m_slot] = false;
    if (result != 0) {
        DebugMsg("Failed to delete file\n");
        return false;
    }
    return true;
}

bool CMemoryCard::DeletePrefFile(int slot) {
    char fileName[36];
    if (MountPortIfRequired(slot) != 1)
        return false;
    sprintf(fileName, "%sprefs", "MOHFL");
    long result = CARDDelete(m_slot, fileName);
    CARDUnmount(slot);
    m_mounted[slot] = false;
    if (result != 0) {
        DebugMsg("Failed to delete file\n");
        return false;
    }
    return true;
}

bool CMemoryCard::Open(unsigned int index, bool) {
    char fileName[36];
    CARDStat stat;
    unsigned char buffer[8192] __attribute__((aligned(32)));
    long result;
    if (MountPortIfRequired(m_slot) != 1)
        return false;
    sprintf(fileName, "MOHFLx%03d", index);
    if (CARDOpen(m_slot, fileName, &m_fileInfo) != 0)
        return false;
    if (CARDGetStatus(m_slot, m_fileInfo.fileNo, &stat) < 0)
        DebugMsg("Failed to set CARDStat in CreateNewSaveGame\n");
    result = CARDReadAsync(&m_fileInfo, buffer, m_sectorSize, 0, 0);
    if (result < 0) {
        DebugMsg("Data read failed with error code %d\n", result);
        return false;
    }
    while ((result = CARDGetResultCode(m_slot)) == -1)
        ;
    if (result < 0) {
        DebugMsg("Data read failed with error code %d\n", result);
        return false;
    }
    strncpy(m_name, (char*)buffer + 32, 32);
    return true;
}

bool CMemoryCard::Close() {
    long result;

    if (!m_mounted[m_slot])
        return true;
    result = CARDClose(&m_fileInfo);
    CARDUnmount(m_slot);
    m_mounted[m_slot] = false;
    if (result != 0) {
        DebugMsg("File failed to close with error %d\n", result);
        return false;
    }
    DebugMsg("File closed successfuly\n");
    return true;
}

bool CMemoryCard::Read(unsigned char* data, unsigned int size) {
    unsigned char buffer[8192] __attribute__((aligned(32)));
    if (MountPortIfRequired(m_slot) != 1)
        return false;
    long result = CARDGetSectorSize(m_slot, &m_sectorSize);
    if (result < 0) {
        DebugMsg("CARDGetSectorSize() failed. (%d)\n", result);
        return false;
    }
    result = CARDReadAsync(&m_fileInfo, buffer, m_sectorSize, m_sectorSize, 0);
    if (result < 0) {
        DebugMsg("Data read failed with error code %d\n", result);
        return false;
    }
    do {
        result = CARDGetResultCode(m_slot);
    } while (result == -1);
    if (result < 0) {
        DebugMsg("Data read failed with error code %d\n", result);
        return false;
    }
    memcpy(data, buffer, size);
    return true;
}

bool CMemoryCard::Write(unsigned char* data, unsigned int size) {
    unsigned long sectorSize;
    unsigned char buffer[8192] __attribute__((aligned(32)));
    if (MountPortIfRequired(m_slot) != 1)
        return false;
    long result = CARDGetSectorSize(m_slot, &sectorSize);
    if (result < 0) {
        DebugMsg("CARDGetSectorSize() failed. (%d)\n", result);
        return false;
    }
    memcpy(buffer, data, size);
    result = CARDWriteAsync(&m_fileInfo, buffer, sectorSize, sectorSize, 0);
    if (result < 0) {
        DebugMsg("Data write failed with error code %d\n", result);
        return false;
    }
    do {
        result = CARDGetResultCode(m_slot);
    } while (result == -1);
    if (result < 0) {
        DebugMsg("Data write failed with error code %d\n", result);
        return false;
    }
    return true;
}

bool CMemoryCard::CreatePrefs() {
    static char HeaderC8[50][32] = {"Medal Of Honor Frontline\n", "Preference File\n"};
    unsigned long sectorSize;
    char fileName[36];
    CARDStat stat;
    unsigned char buffer[8192] __attribute__((aligned(32)));
    long result;
    if (MountPortIfRequired(m_slot) != 1)
        return false;
    sprintf(fileName, "%sprefs", "MOHFL");
    result = CARDGetSectorSize(m_slot, &sectorSize);
    if (result < 0) {
        DebugMsg("CARDGetSectorSize() failed on error code %d\n", result);
        return false;
    }
    result = CARDCreateAsync(m_slot, fileName, sectorSize * 2, &m_fileInfo, 0);
    if (result < 0) {
        switch (result) {
        case -128:
        case -12:
        case -7:
        case -6:
        case -4:
        case -2:
            break;
        }
        if (result < 0) {
            DebugMsg("Failed prefs file create with error code %d\n", result);
            return false;
        }
    }
    while (CARDGetResultCode(m_slot) == -1)
        ;
    result = CARDGetStatus(m_slot, m_fileInfo.fileNo, &stat);
    if (result < 0) {
        DebugMsg("CARDGetStatus() failed on error code %d\n", result);
        return false;
    }
    CARDSetAttributes(m_slot, m_fileInfo.fileNo, 28);
    memcpy(HeaderC8[2], ICON_TEXTURE_DATA, 1024);
    memcpy(HeaderC8[34], ICON_CLUT_DATA, 512);
    CARDSetCommentAddr(&stat, 0);
    CARDSetIconAddr(&stat, 64);
    CARDSetBannerFormat(&stat, 0);
    CARDSetIconAnim(&stat, 0);
    CARDSetIconFormat(&stat, 0, 1);
    CARDSetIconSpeed(&stat, 0, 3);
    CARDSetIconSpeed(&stat, 1, 0);
    memcpy(buffer, HeaderC8, 1600);
    if (CARDWrite(&m_fileInfo, buffer, stat.length, 0) < 0)
        DebugMsg("Failed to CARDWrite in CreateNewSaveGame.\n");
    strncpy((char*)stat.company, COMPANY_CODENAME, 2);
    strncpy((char*)stat.gameName, GAME_CODENAME, 4);
    if (CARDSetStatus(m_slot, m_fileInfo.fileNo, &stat) < 0)
        DebugMsg("Failed to set CARDStat in CreateNewSaveGame\n");
    return true;
}

bool CMemoryCard::SavePrefs(unsigned char* data, unsigned int size) {
    unsigned long sectorSize;
    char fileName[36];
    unsigned char buffer[8192] __attribute__((aligned(32)));
    long result;
    if (MountPortIfRequired(m_slot) != 1)
        return false;
    sprintf(fileName, "%sprefs", "MOHFL");
    if (CARDOpen(m_slot, fileName, &m_fileInfo) != 0) {
        DebugMsg("No pref file found\n");
        return false;
    }
    result = CARDGetSectorSize(m_slot, &sectorSize);
    if (result < 0) {
        DebugMsg("CARDGetSectorSize() failed. (%d)\n", result);
        return false;
    }
    memcpy(buffer, data, size);
    result = CARDWriteAsync(&m_fileInfo, buffer, sectorSize, sectorSize, 0);
    if (result < 0) {
        DebugMsg("Data write failed with error code %d\n", result);
        Close();
        return false;
    }
    while ((result = CARDGetResultCode(m_slot)) == -1)
        ;
    if (result < 0) {
        DebugMsg("Data write failed with error code %d\n", result);
        Close();
        return false;
    }
    Close();
    return true;
}

bool CMemoryCard::LoadPrefs(unsigned char* data, unsigned int size) {
    unsigned long sectorSize;
    char fileName[36];
    unsigned char buffer[8192] __attribute__((aligned(32)));
    long result;
    if (MountPortIfRequired(m_slot) != 1) {
        DebugMsg("Memory card isn't ready\n");
        return false;
    }
    sprintf(fileName, "%sprefs", "MOHFL");
    result = CARDOpen(m_slot, fileName, &m_fileInfo);
    if (result != 0) {
        DebugMsg("No pref file found: %d\n", result);
        return false;
    }
    result = CARDGetSectorSize(m_slot, &sectorSize);
    if (result < 0) {
        DebugMsg("CARDGetSectorSize() failed. (%d)\n", result);
        return false;
    }
    result = CARDReadAsync(&m_fileInfo, buffer, sectorSize, sectorSize, 0);
    if (result < 0) {
        DebugMsg("Data read failed with error code %d\n", result);
        Close();
        return false;
    }
    while ((result = CARDGetResultCode(m_slot)) == -1)
        ;
    if (result < 0) {
        DebugMsg("Data write failed with error code %d\n", result);
        Close();
        return false;
    }
    Close();
    memcpy(data, buffer, size);
    return true;
}

bool CMemoryCard::ArePrefsPresent() {
    CARDStat stat;
    char name[36];
    CARDFileInfo info;
    CARDUnmount(m_slot);
    m_mounted[m_slot] = false;
    if (MountPortIfRequired(m_slot) != 1)
        return false;
    sprintf(name, "%sprefs", "MOHFL");
    for (int i = 0; i < 127; i++) {
        if (CARDFastOpen(m_slot, i, &info) == 0) {
            if (CARDGetStatus(m_slot, info.fileNo, &stat) == 0 && strstr(stat.fileName, name) &&
                (strncmp((char*)stat.gameName, GAME_CODENAME, 4) == 0 ||
                 strncmp((char*)stat.gameName, GAME_DEBUGNAME, 4) == 0) &&
                (strncmp((char*)stat.company, COMPANY_CODENAME, 2) == 0 ||
                 strncmp((char*)stat.company, COMPANY_DEBUGNAME, 2) == 0)) {
                CARDClose(&info);
                return true;
            }
            CARDClose(&info);
        }
    }
    return false;
}

int CMemoryCard::GetSaveGameOverheadSize(bool) {
    return 2;
}

int CMemoryCard::GetPort() {
    return m_slot;
}
