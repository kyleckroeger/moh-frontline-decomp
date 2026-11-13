// CDeviceManager: the four controller ports, shared by every file that uses it
// (PortReadiness step 3.3). CInputManager derives from it.
//
// Evidence: out of line in the original (devicemgr.cpp): the constructor and destructor,
// Init, Update, UpdateActuators, SetActuatorState, GetDeviceState, IsReady(int), Shutdown, and
// CGCDevice::Update and CGCDevice::UpdateActuators. No virtuals (no table). 0x2430 bytes: four
// 0x90c-byte devices (CInputManager's own fields start at +0x2430).
// Inferred: every field name and type (src/game/devicemgr.cpp's, the matched unit), the
// actuator record's name, and the return types.
#ifndef GAME_CDEVICEMANAGER_H
#define GAME_CDEVICEMANAGER_H

// Inferred: one rumble actuator (devicemgr.cpp's SetActuatorState writes type and level).
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
    // the key table, copied from masterEventTable (189 entries of 12 bytes)
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

#endif
