// CDeviceManager, the functions at the start of the file: the manager holds
// four controller devices; it updates their actuators, sets an actuator's
// state (a level chosen by the actuator type, and a duration), reports a
// device's state while its pad reads without error, updates the pads and
// each device, and initialises each device's port, cleared state and copy of
// the master event table. CGCDevice::UpdateActuators (the first device
// function) runs a device's rumble: nothing while the pad reads with an error,
// motor stopped while the game is paused or a multiplayer level has ended;
// otherwise it counts the actuator's time down (in 1/60 s), ends it at zero
// or for type 0, and pulses the motor: an on phase, then an off phase after
// which the next pulse strength is chosen by type (type 1 fades in, type 2
// fades out, 75 + 180 * level scaled by the remaining time; others use the
// level), clamped (above 230 -> 255, below 25 -> 0) and split into on and off
// times of a 0.1 s period; it switches the motor when the state changed and
// returns whether it ran. CDeviceManager and CGCDevice are named by the
// mangled symbols; the device and actuator layouts, the pad data view and the
// table's type are inferred from offsets. Init clears 12 bytes from offset 10,
// which runs into the first two bytes of the actuator at offset 20. In
// SetActuatorState the cases share one tail that fills in the actuator and
// all paths share one return, written here with a label; the actuator index
// is a local copy of the parameter (the target moves it first).
extern "C" {
void* memset(void*, int, unsigned long);
void* memcpy(void*, const void*, unsigned long);
}

struct PADDATAVIEW {
    unsigned char unknown00[10];
    signed char err;
};

extern "C" {
PADDATAVIEW* PAD_getdataptr(int);
void PAD_update();
int PADControlMotor(int, unsigned long);
}

class Pause {
public:
    bool bIsGamePaused();

    unsigned char unknown00[152];
};

extern Pause g_Pause;
extern bool g_bInMultiplayerMode;
extern int g_bMPEndLevel;

extern unsigned char masterEventTable[];

struct CGCActuatorView {
    long type;
    int duration;
    float level;
    float time;
    float onTime;
    float offTime;
    bool active;
    bool motor;
};

class CGCDevice {
public:
    bool UpdateActuators(float);
    void Update();

    bool m_ready;
    unsigned char unknown01[3];
    int m_port;
    unsigned char m_state[2];
    unsigned char m_cleared[10];
    CGCActuatorView m_actuators[1];
    unsigned char m_events[2268];
};

class CDeviceManager {
public:
    CDeviceManager();
    ~CDeviceManager();
    void UpdateActuators(float);
    bool SetActuatorState(int, long, long, float, float);
    void* GetDeviceState(unsigned long);
    bool IsReady(int);
    void Update();
    void Shutdown();
    void Init();

    CGCDevice m_devices[4];
};

void CDeviceManager::UpdateActuators(float time) {
    for (int i = 0; i < 4; i++)
        m_devices[i].UpdateActuators(time);
}

bool CDeviceManager::SetActuatorState(int device, long type, long actuator, float level, float duration) {
    long index = actuator;
    switch (type) {
    case 0:
        if (index >= 1)
            break;
        m_devices[device].m_actuators[index].level = 0.0f;
        goto set;
    case 1:
    case 2:
        index = 0;
        m_devices[device].m_actuators[index].level = level;
        goto set;
    case 9:
        index = 0;
        m_devices[device].m_actuators[index].level = 100.0f;
        goto set;
    case 10:
        index = 0;
        m_devices[device].m_actuators[index].level = 177.0f;
        goto set;
    case 11:
    case 12:
        index = 0;
        m_devices[device].m_actuators[index].level = 255.0f;
        goto set;
    case 13:
        if (index >= 1)
            break;
        m_devices[device].m_actuators[index].level = (unsigned int)(255.0f * level);
    set:
        m_devices[device].m_actuators[index].type = type;
        m_devices[device].m_actuators[index].duration = duration;
        m_devices[device].m_actuators[index].time = duration;
        m_devices[device].m_actuators[index].onTime = 0.0f;
        m_devices[device].m_actuators[index].offTime = 0.0f;
        m_devices[device].m_actuators[index].active = true;
        break;
    }
    return true;
}

void* CDeviceManager::GetDeviceState(unsigned long index) {
    if (PAD_getdataptr(m_devices[index].m_port)->err == 0)
        return m_devices[index].m_state;
    return 0;
}

bool CDeviceManager::IsReady(int index) {
    return m_devices[index].m_ready;
}

void CDeviceManager::Update() {
    PAD_update();
    for (int i = 0; i < 4; i++)
        m_devices[i].Update();
}

void CDeviceManager::Shutdown() {
}

void CDeviceManager::Init() {
    for (int i = 0; i < 4; i++) {
        m_devices[i].m_port = i;
        memset(m_devices[i].m_cleared, 0, 12);
        m_devices[i].m_ready = false;
        memcpy(m_devices[i].m_events, masterEventTable, 2268);
    }
}

CDeviceManager::~CDeviceManager() {
}

CDeviceManager::CDeviceManager() {
}

bool CGCDevice::UpdateActuators(float time) {
    bool changed = false;
    float dt = time / 60.0f;
    if (PAD_getdataptr(m_port)->err)
        return false;
    if (g_Pause.bIsGamePaused() || (g_bInMultiplayerMode && g_bMPEndLevel)) {
        if (m_actuators[0].motor) {
            m_actuators[0].motor = false;
            PADControlMotor(m_port, 2);
        }
        return false;
    }
    if (!m_actuators[0].active)
        return false;
    m_actuators[0].time -= dt;
    if (m_actuators[0].time <= 0.0f || m_actuators[0].type == 0) {
        changed = true;
        m_actuators[0].active = false;
        m_actuators[0].motor = false;
    } else if (m_actuators[0].onTime > 0.0f) {
        m_actuators[0].onTime -= dt;
        if (m_actuators[0].onTime <= 0.0f) {
            m_actuators[0].onTime = 0.0f;
            if (m_actuators[0].offTime > 0.0f) {
                changed = true;
                m_actuators[0].motor = false;
            }
        }
    } else {
        m_actuators[0].offTime -= dt;
        if (m_actuators[0].offTime <= 0.0f) {
            float level;
            switch (m_actuators[0].type) {
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
                level = m_actuators[0].level;
                break;
            case 1: {
                float full = 180.0f * m_actuators[0].level;
                unsigned int start = full;
                level = start + 75 - (unsigned int)(full * (m_actuators[0].time / (float)m_actuators[0].duration));
                break;
            }
            case 2:
                level = (unsigned int)(180.0f * m_actuators[0].level * (m_actuators[0].time / (float)m_actuators[0].duration)) + 75;
                break;
            default:
                level = m_actuators[0].level;
                break;
            }
            if (level > 230.0f)
                level = 255.0f;
            if (level < 25.0f)
                level = 0.0f;
            changed = true;
            m_actuators[0].onTime = 0.1 / 255.0 * level;
            m_actuators[0].offTime = 0.1 - m_actuators[0].onTime;
            m_actuators[0].motor = true;
        }
    }
    if (changed) {
        if (m_actuators[0].motor == true)
            PADControlMotor(m_port, 1);
        else if (m_actuators[0].type == 0)
            PADControlMotor(m_port, 2);
        else
            PADControlMotor(m_port, 0);
    }
    return true;
}
