// A fragment of input.cpp; the file name is this project's. CInputManager, the functions at the start of the file: per-player key
// value tables, rumble feedback and keymaps. CInputManager, CDeviceManager and
// KEYMAP are named by the mangled symbols; the base-class relationship, the
// 2316-byte device record and the key and shell views are inferred from
// offsets and are not original names. The unit is compiled with
// -inline deferred,auto: GetValueAnyPlayer inlines GetValue, which follows it
// in the image, so the functions are listed in reverse image order.
/* inferred: a keymap entry maps a pad input (button bit, analog byte or
   button pair) to a key; 0xffff ends the list or leaves the key unmapped */
struct KEYMAPENTRYVIEW {
    unsigned short key;
    unsigned short input;
};

struct KEYMAP {
    int unknown0;
    KEYMAPENTRYVIEW entries[1];
};

/* inferred: the pad state's button word and analog bytes */
struct InputPadView {
    unsigned short buttons;
    unsigned char analog[10];
};

struct InputKeyView {
    unsigned short flags;
    unsigned char deadZone;
    unsigned char field03;
    int field04;
    int value;
};

struct InputKeyTableView {
    InputKeyView keys[189];
};

struct InputDeviceView {
    unsigned char field0000[48];
    InputKeyTableView table;
};

struct ShellPlayerView {
    unsigned char field00[2];
    bool rumble;
    unsigned char field03[25];
};

struct ShellView {
    unsigned char field0000[5216];
    ShellPlayerView players[4];
};

extern ShellView g_Shell;
extern bool g_bInMultiplayerMode;
extern bool g_unplugged;

template <class T> T MathFunClamp(T, T, T);

extern "C" void PADControlMotor(int, unsigned long);



#include "game/CInputManager.h"


/* inferred: the device's key table (CGCDevice's event table, +0x30) */
static inline InputKeyTableView* GetKeyTable(CDeviceManager* m, int player) { return (InputKeyTableView*)m->m_devices[player].m_events; }

/* inferred: inputs 3 and 2 only drive keys 142 and 143 for limited players */
static inline bool Blocked(int input, unsigned long index) {
    return (input == 3 && index != 142) || (input == 2 && index != 143);
}

static inline int Both(unsigned short b, unsigned int x, unsigned int y) { return ((b & (1 << x)) != 0) & ((b & (1 << y)) != 0); }

void CInputManager::Update(float dt) {

    unsigned int analog;
    unsigned short flags;
    field2444 = 0;
    m_deviceMgr.Update();
    for (int player = 0; player < 4; player++) {
        InputKeyTableView* table = GetKeyTable(&m_deviceMgr, player);
        InputPadView* pad = (InputPadView*)m_deviceMgr.GetDeviceState(player);
        unsigned long j;
        if (table && !pad) {
            for (j = 0; j < 189; j++) {
                table->keys[j].field04 = 0;
                table->keys[j].value = 0;
            }
            continue;
        }
        for (j = 0; j < 189; j++)
            table->keys[j].value = 0;
        if (g_unplugged) {
            for (j = 0; j < 189; j++) {
                table->keys[j].field04 = 0;
                table->keys[j].value = 0;
            }
            continue;
        }
        if (g_bInMultiplayerMode)
            m_keymap = m_playerKeymaps[player];
        if (!m_keymap)
            continue;
        for (int i = 0;; i++) {
            unsigned long index = m_keymap->entries[i].key;
            if (index == 0xffff)
                break;
            unsigned short input = m_keymap->entries[i].input;
            if (input == 0xffff) {
                table->keys[index].value = 0;
                continue;
            }
            flags = table->keys[index].flags;
            InputKeyView* key = &table->keys[index];
            analog = flags & 1;
            unsigned char raw;
            if (analog) {
                raw = pad->analog[input];
            } else if (flags & 0x10) {
                raw = Both(pad->buttons, input >> 8, input & 0xff);
            } else {
                if (m_zooming[player] && (((int)input == 3 && index != 142) || ((int)input == 2 && index != 143))) {
                    key->value = 0;
                    continue;
                }
                raw = (pad->buttons & (1 << input)) != 0;
            }
            int value = raw;
            unsigned int sign = flags & 4;
            if (sign)
                value -= 128;
            if (analog) {
                int deadZone = key->deadZone;
                int range = sign ? 128 : 255;
                if (input >= 8)
                    value <<= 1;
                if (value > 0) {
                    value = range * (value - deadZone) / (range - deadZone);
                    if (value < 0)
                        value = 0;
                } else {
                    value = range * (value + deadZone) / (range - deadZone);
                    if (value > 0)
                        value = 0;
                }
                if (sign)
                    value = MathFunClamp<long>(value, -128, 127);
                else
                    value = MathFunClamp<long>(value, 0, 255);
            }
            if (flags & 8) {
                int direction = 1;
                if (value < 0) {
                    direction = -1;
                    value *= -1;
                }
                if (analog)
                    value = value * (value * (direction * value)) / 16129;
                else
                    value = value * (value * (direction * value)) / 65025;
            }
            if (flags & 2) {
                int previous = key->field04;
                key->field04 = value;
                bool edge = value != 0 && value != previous;
                value = edge != 0;
            }
            if (value != 0)
                field2444 = 1;
            key->value = value;
        }
    }
    m_deviceMgr.UpdateActuators(dt);
}

void CInputManager::Shutdown() {
    m_deviceMgr.Shutdown();
}

void CInputManager::SetKeymap(unsigned long player, KEYMAP* keymap) {
    if (g_bInMultiplayerMode)
        m_playerKeymaps[player] = keymap;
    else
        m_keymap = keymap;
}

int CInputManager::GetValue(unsigned long player, unsigned long key) {
    InputKeyTableView* table = GetKeyTable(&m_deviceMgr, player);
    return table ? table->keys[key].value : 0;
}

bool CInputManager::GetValueAnyPlayer(unsigned long key) {
    bool down = GetValue(0, key) != 0;
    if (!down) down = GetValue(1, key) != 0;
    if (!down) down = GetValue(2, key) != 0;
    if (!down) down = GetValue(3, key) != 0;
    return down;
}

void CInputManager::ResetActuatorState(long state) {
    for (int i = 0; i < 4; i++) {
        m_deviceMgr.SetActuatorState(i, state, 0, 0.0f, 1.0f);
        if (state == 0)
            PADControlMotor(i, 2);
    }
}

void CInputManager::SetFeedback(unsigned long player, long a, long b, float c, float d) {
    bool enabled = false;

    if (g_bInMultiplayerMode) {
        if (g_Shell.players[player].rumble)
            enabled = true;
    } else {
        enabled = m_feedback;
    }
    if (enabled && !m_feedbackOff[player])
        m_deviceMgr.SetActuatorState(player, a, b, c, d);
}

bool CInputManager::IsReady() {
    for (int i = 0; i < 4; i++) {
        if (m_deviceMgr.IsReady(i))
            return true;
    }
    return false;
}

bool CInputManager::VerifyAnalog() {
    return 1;
}

void CInputManager::Reset() {
    InputDeviceView* device = (InputDeviceView*)m_deviceMgr.m_devices;
    InputKeyTableView* table;
    int i;

    for (i = 0; i < 4; i++, device++) {
        table = &device->table;
        if (table) {
            for (int j = 0; j < 189; j++) {
                table->keys[j].value = 0;
                table->keys[j].field04 = 0;
            }
        }
    }
}
