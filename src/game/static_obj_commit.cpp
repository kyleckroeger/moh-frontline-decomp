// A fragment of static_obj.cpp (0x800b031c): CStaticObject's
// CommitPathMotionUpdate (saves the transform's position as the path motion's
// position, copies a speed value and recomputes the path destination and
// speed) and CommitAnimMotionUpdate (while animation motion is active, saves
// the position and sets the rotation at +336 from the transform) and
// CommitScriptMotionUpdate (saves the position, then for each active script
// motion - flag bits at +480 - steps its time down by its step, never below
// 0 (a .sdata2 pool entry), the rotation one also taking its target rotation
// and the value one advancing its value by step times rate). CStaticObject,
// CMatrix and CQuaternion are named by the mangled symbols; CStaticObject is
// an inferred non-virtual view (the script-motion members are named by
// offset), and the CVector3 view and by-value position
// getter are as in static_obj_setpos.cpp. The rest of the file is not part of
// this unit.
class CVector3 {
public:
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        double pair[2];
    };
} __attribute__((aligned(8)));

class CMatrix {
public:
    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);

    float x;
    float y;
    float z;
    float w;
};

class CStaticObject {
public:
    void CommitPathMotionUpdate();
    void CommitAnimMotionUpdate();
    void CommitScriptMotionUpdate();
    void UpdatePathMotionDestAndSpeed();

    CVector3 PositionRow() const { return m_tm.position; }

    unsigned char unknown000[64];
    CMatrix m_tm;
    unsigned char unknown080[88];
    float m_speed216;
    unsigned char unknown0dc[68];
    CVector3 m_animPosition;
    unsigned char unknown130[32];
    CQuaternion m_animRotation;
    unsigned char unknown160[16];
    CVector3 m_pathPosition;
    CQuaternion m_scriptRotation;
    unsigned char unknown190[16];
    CQuaternion m_scriptTargetRotation;
    float m_value1b0;
    float m_value1b4;
    float m_time1b8;
    float m_time1bc;
    float m_time1c0;
    unsigned char unknown1c4[12];
    float m_step1d0;
    float m_step1d4;
    float m_step1d8;
    unsigned char unknown1dc[4];
    unsigned char m_flag80 : 1;
    unsigned char m_scriptMove : 1;
    unsigned char m_scriptRotate : 1;
    unsigned char m_scriptValue : 1;
    unsigned char m_flags0f : 4;
    unsigned char unknown1e1[23];
    float m_pathSpeed;
    unsigned char unknown1fc[40];
    int m_animMotion;
    unsigned char unknown228[4];
    int m_animMotionActive;
};

void CStaticObject::CommitPathMotionUpdate() {
    m_pathPosition = PositionRow();
    m_pathSpeed = m_speed216;
    UpdatePathMotionDestAndSpeed();
}

void CStaticObject::CommitAnimMotionUpdate() {
    if (m_animMotion && m_animMotionActive) {
        m_animPosition = PositionRow();
        m_animRotation.SetFromMatrix(m_tm);
    }
}

void CStaticObject::CommitScriptMotionUpdate() {
    m_pathPosition = PositionRow();
    if (m_scriptMove) {
        m_time1b8 -= m_step1d0;
        m_time1b8 = m_time1b8 < 0.0f ? 0.0f : m_time1b8;
    }
    if (m_scriptRotate) {
        m_scriptRotation = m_scriptTargetRotation;
        m_time1bc -= m_step1d4;
        m_time1bc = m_time1bc < 0.0f ? 0.0f : m_time1bc;
    }
    if (m_scriptValue) {
        m_value1b4 += m_step1d8 * m_value1b0;
        m_time1c0 -= m_step1d8;
        m_time1c0 = m_time1c0 < 0.0f ? 0.0f : m_time1c0;
    }
}
