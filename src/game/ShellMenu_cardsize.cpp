// A fragment of ShellMenu.cpp (0x800e3ff0): MemoryCardSlotOkForStartup
// checks a card slot at start-up and reports why it is not usable (sets all
// the out flags to their defaults, then: no card -> not present, fine;
// already skipped -> fine; cannot open, wrong encoding, bad format state (2
// or 3) or unformatted -> fail; otherwise checks the card size, records
// whether saves exist, and fails with the bytes or files still needed when
// the preferences (if missing) and a save game do not fit). Then CShellMenu's
// memory-card space queries: the free space on the card in a port (0 unless the port is
// current or opens) and the save sizes of the preferences and the game (both
// the card's overhead size with the flag clear). CShellMenu, CMemoryCard and
// the functions are named by the mangled symbols; the card member (+6412),
// the port selection inline (as in ShellMenu_card.cpp), the result types
// and the start-up check's parameter meanings are inferred. The rest of the file is not part of this unit.
class CMemoryCard {
public:
    int GetPort();
    bool OpenSlot(int);
    int GetAvailableMemory();
    int GetSaveGameOverheadSize(bool);
    bool QuerySlot(int);
    bool IsEncodingCorrect(int);
    int IsFormated();
    long GetCheckCardStatus();
    bool Is8KCard(int);
    int GetNumSaveGames();
    bool ArePrefsPresent();
    unsigned long GetFilesFree(int);
};

bool MemoryCardSlotOkForStartup(CMemoryCard* card, int slot, int prefsSize, int saveSize, int* bytesNeeded,
                                bool* unformatted, int* filesNeeded, bool* encodingOk, bool* is8K, bool* present,
                                bool* skip, int* formatStatus, bool* opened, bool* hasSaves) {
    int extraFiles = 0;
    *filesNeeded = 0;
    *bytesNeeded = 0;
    *encodingOk = true;
    *is8K = true;
    *present = true;
    *formatStatus = 0;
    *opened = true;
    *hasSaves = true;
    *unformatted = false;
    if (!card->QuerySlot(slot)) {
        *present = false;
        *skip = false;
        return true;
    }
    if (*skip)
        return true;
    if (!card->OpenSlot(slot)) {
        *opened = false;
        return false;
    }
    if (!card->IsEncodingCorrect(slot)) {
        *encodingOk = false;
        return false;
    }
    int format = card->IsFormated();
    if (format == 2 || format == 3) {
        *formatStatus = format;
        return false;
    }
    if (format != 0) {
        int needed = 0;
        card->GetCheckCardStatus();
        if (!card->Is8KCard(slot)) {
            *is8K = false;
            return false;
        }
        bool saves = card->GetNumSaveGames() != 0;
        if (saves)
            *hasSaves = true;
        else
            *hasSaves = false;
        if (!card->ArePrefsPresent()) {
            needed = prefsSize;
            extraFiles = 1;
            if ((unsigned long)card->GetAvailableMemory() < 2 || card->GetFilesFree(slot) < 1)
                *hasSaves = false;
        }
        needed += saveSize;
        if (card->GetAvailableMemory() < needed) {
            *bytesNeeded = needed;
            return false;
        }
        if (card->GetFilesFree(slot) < extraFiles + 1) {
            *filesNeeded = extraFiles + 1;
            return false;
        }
    } else {
        *unformatted = true;
        return false;
    }
    return true;
}

class CShellMenu {
public:
    int MemoryCardFreeSpace(int);
    int MemoryCardSavePrefsSize();
    int MemoryCardSaveGameSize();

    bool SelectCardPort(int port) {
        if (port != m_card->GetPort() && !m_card->OpenSlot(port))
            return false;
        return true;
    }

    unsigned char unknown0000[6412];
    CMemoryCard* m_card;
};

int CShellMenu::MemoryCardFreeSpace(int port) {
    if (!SelectCardPort(port))
        return 0;
    return m_card->GetAvailableMemory();
}

int CShellMenu::MemoryCardSavePrefsSize() {
    return m_card->GetSaveGameOverheadSize(false);
}

int CShellMenu::MemoryCardSaveGameSize() {
    return m_card->GetSaveGameOverheadSize(false);
}
