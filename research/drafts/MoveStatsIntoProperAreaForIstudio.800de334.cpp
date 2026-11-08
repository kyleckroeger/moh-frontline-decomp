extern "C" {
void* memcpy(void*, const void*, unsigned long);
void* memset(void*, int, unsigned long);
char* strcpy(char*, const char*);
}

class CDeviceManager {
public:
    bool IsReady(int);

    unsigned char unknown00[64];
};

extern CDeviceManager g_inputMgr;

struct MPPlayerSettingsView {
    int teamColor;
    int model;
    char name[11];
    unsigned char advanced[5];
    unsigned char controller;
    unsigned char unknown19[3];
};

struct MPStatsView {
    int shots;
    int hits[7];
    unsigned char unknown20[196];
    int stats[5];
    unsigned char unknownf8[8];
};

struct SoundOptionsView {
    unsigned char unknown0[2];
    signed char mode;
};

class CShellMenu {
public:
    int CountWindows() {
        int count = 0;
        if (m_activeWindows[0] != 0)
            count++;
        if (m_activeWindows[1] != 0)
            count++;
        if (m_activeWindows[2] != 0)
            count++;
        if (m_activeWindows[3] != 0)
            count++;
        m_activeWindowCount = count;
        return count;
    }
    void MoveStatsIntoProperAreaForIstudio();
    void Copy(MPStatsView* dest, int i) { if (m_activeWindows[i]) memcpy(dest, m_mpStats + i, sizeof(MPStatsView)); }

    unsigned char unknown0000[5140];
    int m_activeWindowCount;
    unsigned char unknown1418[180];
    MPStatsView m_mpStats[4];
    int m_activeWindows[4];
};

void CShellMenu::MoveStatsIntoProperAreaForIstudio() {
    int count = CountWindows();
    if (count < 2 || count > 3)
        return;
    {
        MPStatsView stats[4];
        memset(stats, 0, sizeof(stats));
        MPStatsView* dest = stats;
        for (int i = 0; i < count; i++, dest++)
            Copy(dest, i);
        memcpy(m_mpStats, stats, sizeof(stats));
    }
}
