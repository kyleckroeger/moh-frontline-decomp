// ShellMenu.cpp: CShellMenu::MemoryCardSaveGame. Class layouts are inferred views of what this function
// uses. ShellMenu_block.h recreates the constant block its offsets depend on.
#include "ShellMenu_block.h"

extern "C" {
char* strncpy(char*, const char*, unsigned long);
}
void DebugMsg(const char*, ...);
class CRCModelView {
public:
    CRCModelView() {
        m_width = 32;
        m_poly = 0x04C11DB7;
        m_init = 0xFFFFFFFF;
        m_xorOut = 0xFFFFFFFF;
        m_reflect = 1;
        m_mask = 0xFFFFFFFF;
        m_reg = m_init;
    }
    static unsigned long Reflect(unsigned long v, unsigned long bits) {
        unsigned long t = v;
        for (int i = 0; i < bits; i++) {
            if (t & 1)
                v |= 1 << ((bits - 1) - i);
            else
                v &= ~(1 << ((bits - 1) - i));
            t >>= 1;
        }
        return v;
    }
    void Next(int ch) {
        unsigned long uch = (unsigned long)ch;
        unsigned long topbit = 1 << (m_width - 1);
        if (m_reflect)
            uch = Reflect(uch, 8);
        m_reg ^= uch << (m_width - 8);
        for (int i = 0; i < 8; i++) {
            if (m_reg & topbit)
                m_reg = (m_reg << 1) ^ m_poly;
            else
                m_reg <<= 1;
            m_reg &= m_mask;
        }
    }
    void Block(unsigned char* p, unsigned long len) {
        while (len--)
            Next(*p++);
    }
    unsigned long Crc() {
        if (m_reflect)
            return m_xorOut ^ Reflect(m_reg, m_width);
        return m_xorOut ^ m_reg;
    }

    unsigned long m_width;
    unsigned long m_mask;
    unsigned long m_poly;
    unsigned long m_init;
    unsigned long m_reflect;
    unsigned long m_xorOut;
    unsigned long m_reg;
};


class CMemoryCard {
public:
    int GetPort();
    bool OpenSlot(int);
    int IsFormated();
    bool ArePrefsPresent();
    bool CreatePrefs();
    bool SavePrefs(unsigned char*, unsigned int);
    bool IsSaveGamePresent(int);
    bool DeleteSaveGame(unsigned int);
    bool CreateNewSaveGame(unsigned int*, char*);
    bool Open(unsigned int, bool);
    bool Write(unsigned char*, unsigned int);
    bool Close();
    int GetSaveGameOverheadSize(bool);
    int GetAvailableMemory();
};

class CShellMenu {
public:
    int MemoryCardSaveGame(int, int, char*);
    inline unsigned long DataCRC(unsigned char*, unsigned long) const;
    bool IsSlotOpen(int slot) {
        if (slot != m_card->GetPort() && !m_card->OpenSlot(slot))
            return false;
        return true;
    }
    bool SavePrefsFile(int slot) {
        struct {
            unsigned char file;
            char tag[11];
        } prefs;
        if (!IsSlotOpen(slot))
            return false;
        if (!m_card->ArePrefsPresent())
            m_card->CreatePrefs();
        prefs.file = m_prefsFile;
        strncpy(prefs.tag, "Frontline", 9);
        return m_card->SavePrefs((unsigned char*)&prefs, 12);
    }
    int FreeMemory(int slot) {
        if (IsSlotOpen(slot))
            return m_card->GetAvailableMemory();
        return 0;
    }

    unsigned char m_savedData[6364];
    unsigned long m_crc;
    unsigned char m_afterSaved[44];
    CMemoryCard* m_card;
    unsigned char m_prefsFile;
};

unsigned long CShellMenu::DataCRC(unsigned char* data, unsigned long size) const {
    CRCModelView* crc = new CRCModelView;
    if (!data) {
        data = (unsigned char*)this;
        size = (unsigned char*)&m_crc - (unsigned char*)this;
    }
    for (; size; size--) {
        crc->Next(*data);
        data++;
    }
    unsigned long value = crc->Crc();
    delete crc;
    return value;
}

int CShellMenu::MemoryCardSaveGame(int slot, int file, char* name) {
    if (!IsSlotOpen(slot))
        return 0;
    if (m_card->IsSaveGamePresent(file) && !m_card->DeleteSaveGame(file)) {
        if (m_card->IsFormated() == 3) {
            DebugMsg("Detected Erase Error!\n");
            return -1;
        }
        return 0;
    }
    m_crc = DataCRC(0, 0);
    unsigned int created;
    if (!m_card->CreateNewSaveGame(&created, name)) {
        if (m_card->IsFormated() == 3) {
            DebugMsg("Detected Program Error!\n");
            return -1;
        }
        return 0;
    }
    if (!m_card->Open(created, true))
        return 0;
    if (!m_card->Write((unsigned char*)this, (unsigned char*)&m_afterSaved - (unsigned char*)this)) {
        m_card->Close();
        return 0;
    }
    if (!m_card->Close())
        return 0;
    m_prefsFile = created;
    if (!m_card->ArePrefsPresent()) {
        int size = m_card->GetSaveGameOverheadSize(false);
        if (FreeMemory(slot) < size)
            return 1;
        return SavePrefsFile(slot);
    }
    return SavePrefsFile(slot);
}
