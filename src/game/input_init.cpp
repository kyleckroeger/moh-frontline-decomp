// A fragment of input.cpp (0x80085144); the file name is this project's.
// CInputManager::Init initialises the device manager, clears the keymap, the
// byte at +9284 and the five at +9290, enables feedback, then updates the
// input (for up to 10000 tries) until any device is ready (IsReady, defined
// earlier in the file, inlined; repeated here as an inline copy) and clears
// the per-player keymaps. The view follows input_manager.cpp; the members
// at +9284 and +9290 are inferred. The file is compiled with -inline
// deferred,auto. The rest of the file is in other units.
class CDeviceManager {
public:
    void Init();
    bool IsReady(int);

    unsigned char m_devices[4][2316];
};

struct KEYMAP;

class CInputManager : public CDeviceManager {
public:
    void Init();
    void Update(float);
    inline bool IsReady();

    KEYMAP* m_keymap;
    KEYMAP* m_playerKeymaps[4];
    unsigned char field2444;
    bool m_feedback;
    bool m_feedbackOff[4];
    unsigned char field244a[5];
};

inline bool CInputManager::IsReady() {
    for (int i = 0; i < 4; i++) {
        if (CDeviceManager::IsReady(i))
            return true;
    }
    return false;
}

void CInputManager::Init() {
    CDeviceManager::Init();
    m_keymap = 0;
    field2444 = 0;
    m_feedback = true;
    field244a[0] = 0;
    field244a[1] = 0;
    field244a[2] = 0;
    field244a[3] = 0;
    field244a[4] = 0;
    for (int i = 0; i < 10000; i++) {
        Update(0.0f);
        if (IsReady())
            break;
    }
    m_playerKeymaps[0] = 0;
    m_playerKeymaps[1] = 0;
    m_playerKeymaps[2] = 0;
    m_playerKeymaps[3] = 0;
}
