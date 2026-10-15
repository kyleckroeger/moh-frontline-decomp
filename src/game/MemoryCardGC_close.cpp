/* CMemoryCard::Write, Read and Close (a fragment of MemoryCardGC.cpp).
   Write copies the data into an aligned sector buffer and writes one sector
   (the sector size also passed as the offset, as the code does); Read reads
   one sector and copies it out; both mount the card when required and wait
   for the asynchronous result, reporting failures. Close: close the open save
   file of a mounted card and unmount it, reporting the result. CMemoryCard
   is named by the mangled symbols and CARDFileInfo by the SDK; the members
   are inferred from offsets (slot at +4, file info at +8, mounted flags at
   +290). The return type is inferred. */
struct CARDFileInfo {
    unsigned char unknown00[20];
};

extern "C" {
long CARDClose(CARDFileInfo*);
long CARDUnmount(long);
long CARDGetSectorSize(long, unsigned long*);
long CARDWriteAsync(CARDFileInfo*, void*, long, long, void (*)(long, long));
long CARDReadAsync(CARDFileInfo*, void*, long, long, void (*)(long, long));
long CARDGetResultCode(long);
void* memcpy(void*, const void*, unsigned long);
}

void DebugMsg(const char*, ...);

class CMemoryCard {
public:
    bool Write(unsigned char*, unsigned int);
    bool Read(unsigned char*, unsigned int);
    bool Close();
    int MountPortIfRequired(int);

    unsigned char unknown00000[4];
    int m_slot;
    CARDFileInfo m_fileInfo;
    unsigned long m_sectorSize;
    unsigned char unknown00020[258];
    bool m_mounted[2];
};

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
