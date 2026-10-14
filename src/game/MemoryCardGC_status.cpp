/* CMemoryCard::GetNumSaveGames, GetCheckCardStatus and GetFilesFree (a
   fragment of MemoryCardGC.cpp). GetNumSaveGames clears the per-file flags,
   mounts the slot, opens every directory entry and counts the files that are
   this game's saves (file name containing "MOHFL" but not "prefs", game and
   company code matching the release or debug codes), flagging each one, then
   unmounts the slot. The other two run a card check, polling the result while servicing
   disc errors, and return the free file count of a mounted card, unmounting
   it when the query fails. CMemoryCard is named by the mangled symbols; the
   members are inferred from offsets (slot at +4, per-file save flags at
   +36, a second per-file flag array at +163, per-slot mounted flags at
   +290), as are the views of the SDK's file info and status records. The card functions take the SDK's s32 (long). */
extern "C" {
long CARDCheckAsync(long, void*);
long CARDGetResultCode(long);
long CARDFreeBlocks(long, long*, long*);
long CARDUnmount(long);
long CARDProbeEx(long, long*, long*);
long CARDProbe(long);
long CARDFastOpen(long, long, void*);
long CARDGetStatus(long, long, void*);
long CARDClose(void*);
char* strstr(const char*, const char*);
int strncmp(const char*, const char*, unsigned long);
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

void DebugMsg(const char*, ...);
void MohDVDErrorTask(int, int);

class CMemoryCard {
public:
    int GetNumSaveGames();
    long GetCheckCardStatus();
    long GetFilesFree(int);
    int GetMaxBlocksOnCard(int);
    int MountPortIfRequired(int);
    bool Is8KCard(int);
    bool IsAnyDeviceInPort(int);

    unsigned char unknown00000[4];
    int m_slot;
    unsigned char unknown00008[28];
    bool m_isSave[127];
    bool m_unknownA3[127];
    bool m_mounted[2];
};

int CMemoryCard::GetNumSaveGames() {
    int count;
    int i;
    count = 0;
    for (i = 0; i < 127; i++) {
        m_isSave[i] = false;
        m_unknownA3[i] = false;
    }
    if (MountPortIfRequired(m_slot) != 1)
        return 0;
    for (i = 0; i < 127; i++) {
        CardFileInfoView info;
        CardStatView stat;
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

long CMemoryCard::GetCheckCardStatus() {
    long result;
    long slot;
    CARDCheckAsync(m_slot, 0);
    slot = m_slot;
    while ((result = CARDGetResultCode(slot)) == -1)
        MohDVDErrorTask(0, 0);
    return result;
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
