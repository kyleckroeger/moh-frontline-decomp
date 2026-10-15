/* CMemoryCard::GetPort, GetSaveGameOverheadSize and ArePrefsPresent (the
   start of MemoryCardGC.cpp). ArePrefsPresent unmounts and remounts the
   slot, builds the prefs file name ("MOHFL" + "prefs") and looks for an
   open-able directory entry whose name contains it and whose game and
   company codes match the release or debug codes. CMemoryCard is named by
   the mangled symbols; the members and the SDK record views follow
   MemoryCardGC_status.cpp (slot at +4, mounted flags at +290). The overhead
   size's parameter is unused. The card functions take the SDK's s32
   (long). */
extern "C" {
long CARDUnmount(long);
long CARDFastOpen(long, long, void*);
long CARDGetStatus(long, long, void*);
long CARDClose(void*);
char* strstr(const char*, const char*);
int strncmp(const char*, const char*, unsigned long);
int sprintf(char*, const char*, ...);
}

/* inferred views of the SDK's CARDFileInfo and CARDStat */
struct CardFileInfoView {
    long chan;
    long fileNo;
    long offset;
    long length;
    unsigned short iBlock;
};

struct CardStatView {
    char fileName[32];
    unsigned long length;
    unsigned long time;
    unsigned char gameName[4];
    unsigned char company[2];
    unsigned char unknown2e[62];
};

extern char* GAME_CODENAME;
extern char* GAME_DEBUGNAME;
extern char* COMPANY_CODENAME;
extern char* COMPANY_DEBUGNAME;

class CMemoryCard {
public:
    int GetPort();
    int GetSaveGameOverheadSize(bool);
    bool ArePrefsPresent();
    int MountPortIfRequired(int);

    unsigned char unknown00000[4];
    int m_slot;
    unsigned char unknown00008[282];
    bool m_mounted[2];
};

int CMemoryCard::GetPort() {
    return m_slot;
}

int CMemoryCard::GetSaveGameOverheadSize(bool) {
    return 2;
}

bool CMemoryCard::ArePrefsPresent() {
    CardStatView stat;
    char name[36];
    CardFileInfoView info;
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
