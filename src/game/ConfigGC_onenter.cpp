// A fragment of ConfigGC.cpp (0x800f93ac): CDualConfig::OnEnter fetches the
// 13 button bytes of the current configuration (+64: the shell's custom
// configurations above 5, else the defaults), copies them into both config
// texts, records the shell's controller setting (+52; with setting 2 the
// ninth byte becomes 7), terminates both texts and clears +20 and +14.
// CDualConfig, CShellMenu, the functions and the two texts are named by the
// symbols; the members, the 13-byte buffer and g_Shell's size are inferred.
// SetUpDefaultConfig writes one of five fixed button layouts into the
// current config text (other values leave it) and applies the changes;
// GetAppliedState and SetAppliedState read and write the flag at +12.
// The rest of the file is in other units.
class CShellMenu {
public:
    void RetrieveShellControllerButtons(char*, int);
    unsigned char Get_currentControllerSetting();

    unsigned char unknown0000[0x19dc];
};

extern CShellMenu g_Shell;
extern char currConfigText[14];
extern char visualConfigText[14];

class CDualConfig {
public:
    void OnEnter();
    void SetUpDefaultConfig(int);
    bool GetAppliedState();
    void SetAppliedState(bool);
    void CopyDefaultConfig(int, char*);
    void ApplyChanges();

    unsigned char unknown00[12];
    bool m_applied;
    unsigned char unknown0d;
    bool m_flag0e;
    unsigned char unknown0f[5];
    int m_unknown14;
    unsigned char unknown18[28];
    int m_controllerSetting;
    unsigned char unknown38[8];
    int m_config;
};

void CDualConfig::OnEnter() {
    char buttons[13];
    if (m_config > 5)
        g_Shell.RetrieveShellControllerButtons(buttons, m_config - 6);
    else
        CopyDefaultConfig(m_config, buttons);
    for (int i = 0; i < 13; i++) {
        currConfigText[i] = buttons[i];
        visualConfigText[i] = buttons[i];
    }
    m_controllerSetting = g_Shell.Get_currentControllerSetting();
    if (m_controllerSetting == 2) {
        currConfigText[8] = 7;
        visualConfigText[8] = 7;
    }
    currConfigText[13] = 0;
    visualConfigText[13] = 0;
    m_unknown14 = 0;
    m_flag0e = false;
}

void CDualConfig::SetUpDefaultConfig(int config) {
    switch (config) {
    case 1:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 2;
        currConfigText[3] = 0;
        currConfigText[4] = 0;
        currConfigText[5] = 1;
        currConfigText[6] = 4;
        currConfigText[7] = 5;
        currConfigText[8] = 6;
        currConfigText[9] = 9;
        currConfigText[10] = 8;
        currConfigText[11] = 3;
        currConfigText[12] = 2;
        break;
    case 2:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 0;
        currConfigText[3] = 4;
        currConfigText[4] = 0;
        currConfigText[5] = 9;
        currConfigText[6] = 6;
        currConfigText[7] = 2;
        currConfigText[8] = 3;
        currConfigText[9] = 8;
        currConfigText[10] = 7;
        currConfigText[11] = 1;
        currConfigText[12] = 3;
        break;
    case 3:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 2;
        currConfigText[3] = 0;
        currConfigText[4] = 0;
        currConfigText[5] = 1;
        currConfigText[6] = 4;
        currConfigText[7] = 5;
        currConfigText[8] = 6;
        currConfigText[9] = 9;
        currConfigText[10] = 8;
        currConfigText[11] = 2;
        currConfigText[12] = 3;
        break;
    case 4:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 2;
        currConfigText[3] = 0;
        currConfigText[4] = 0;
        currConfigText[5] = 1;
        currConfigText[6] = 4;
        currConfigText[7] = 5;
        currConfigText[8] = 2;
        currConfigText[9] = 9;
        currConfigText[10] = 8;
        currConfigText[11] = 6;
        currConfigText[12] = 3;
        break;
    case 5:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 0;
        currConfigText[3] = 2;
        currConfigText[4] = 0;
        currConfigText[5] = 1;
        currConfigText[6] = 4;
        currConfigText[7] = 5;
        currConfigText[8] = 6;
        currConfigText[9] = 9;
        currConfigText[10] = 8;
        currConfigText[11] = 3;
        currConfigText[12] = 2;
        break;
    }
    ApplyChanges();
}

bool CDualConfig::GetAppliedState() {
    return m_applied;
}

void CDualConfig::SetAppliedState(bool applied) {
    m_applied = applied;
}
