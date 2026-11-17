// A fragment of ShellMenu.cpp (0x800df3c8): NullOutJournalSpritesFromLibrary
// (nothing on journal page 6; otherwise deletes the current journal entry's sprites
// from the library's sprite table and clears their slots, the entry's first
// sprite being the sum of the counts of the entries before it; then marks the
// page and entry as none) and CShellMenu's audio options (three signed byte
// settings in a record at +40, sound effects first, then music) passed to the
// interface studio. The journal members and the library view are inferred. The record's inline accessors check their
// this pointer (-1 for a missing record), as the target's null checks of the
// record's address show. CShellMenu and the function are named by the mangled
// symbols; the member, the record view and the result type are inferred.
// ResetAudio after this and the rest of the file are not part of this unit
// (the music and sound-effect setters are in ShellMenu_audioset.cpp).
extern "C" void* memcpy(void*, const void*, unsigned long);

// CRenderBin view as in UserInterface.cpp: 28 bytes of members, then the
// virtual table pointer with the virtual destructor first.
#include "game/CRenderBin.h"


class CSprite : public CRenderBin {
public:
    virtual ~CSprite();
};

// Inferred: the current library's sprite table at +140.
struct LibraryInfoView {
    unsigned char unknown00[140];
    int* sprites;
};

extern LibraryInfoView* LibraryInfo;

struct AudioOptionsView {
    signed char m_settings[3];

    signed char Get(int index) { return this == 0 ? -1 : m_settings[index]; }
};

class CShellMenu {
public:
    void NullOutJournalSpritesFromLibrary();
    int PassAudioSettingToIStudio(int*);

    unsigned char unknown0000[40];
    AudioOptionsView m_audio;
    unsigned char unknown002b[6527];
    signed char m_journalPage;
    signed char m_journalEntry;
    signed char m_journalSpriteCounts[24];
};

void CShellMenu::NullOutJournalSpritesFromLibrary() {
    signed char count;
    int i;
    int entry;
    signed char first;

    if (m_journalPage == 6)
        return;
    entry = m_journalEntry - 1 + (m_journalPage - 1) * 4;
    first = 0;
    for (i = 0; i < entry; i++)
        first += m_journalSpriteCounts[i];
    count = m_journalSpriteCounts[entry];
    for (i = 0; i < count; i++) {
        if (LibraryInfo->sprites[first + i]) {
            delete (CSprite*)LibraryInfo->sprites[first + i];
            LibraryInfo->sprites[first + i] = 0;
        }
    }
    m_journalPage = -1;
    m_journalEntry = -1;
}

int CShellMenu::PassAudioSettingToIStudio(int* out) {
    int settings[3];

    settings[0] = m_audio.Get(0);
    settings[1] = m_audio.Get(1);
    settings[2] = m_audio.Get(2);
    memcpy(out, settings, sizeof(settings));
    return 3;
}
