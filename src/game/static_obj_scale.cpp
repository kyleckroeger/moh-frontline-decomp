// A fragment of static_obj.cpp (0x800af468): CStaticObject::ScaleFromScript
// starts a scripted value change: the script-value flag is set, the event to
// fire is kept, and the target value and the time divided by it are stored.
// The class, function and EBSEventEnum are named by the symbols; the members
// follow static_obj_attempt.cpp (their meanings inferred). The rest of the
// file is in other units.
enum EBSEventEnum {};

class CStaticObject {
public:
    void ScaleFromScript(float, float, EBSEventEnum);

    unsigned char unknown000[432];
    float m_value1b0;
    unsigned char unknown1b4[12];
    float m_time1c0;
    unsigned char unknown1c4[8];
    int m_valueEvent;
    unsigned char unknown1d0[16];
    unsigned char m_flag80 : 1;
    unsigned char m_scriptMove : 1;
    unsigned char m_scriptRotate : 1;
    unsigned char m_scriptValue : 1;
    unsigned char m_flags1e0 : 4;
};

void CStaticObject::ScaleFromScript(float value, float time, EBSEventEnum event) {
    m_scriptValue = true;
    m_valueEvent = event;
    m_value1b0 = value;
    m_time1c0 = time / value;
}
