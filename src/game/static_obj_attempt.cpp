// A fragment of static_obj.cpp (0x800b0560): CStaticObject's
// AttemptPathMotionUpdate (steers the transform toward the path destination
// with yaw and pitch rates that ease off before the target heading, and
// slows the path speed while turning), AttemptAnimMotionUpdate (while animation motion is active, turns the
// transform part way to the target rotation and moves it by the velocity
// from the animated position), AttemptScriptMotionUpdate (steps each active
// script motion - move, turn, scale value - by at most its remaining time,
// rebuilds the transform from the turned rotation, the moved position and a
// scale, and records the steps) and BeginUpdate (updates the light volumes
// and animation, then begins the running path, script or animation motion;
// the script begin ends finished motions with their script events); each
// tells the matrix listener and sets the extent nodes from the listener's
// extents or the position (the begin widens them by 1/64). BeginAnimMotionUpdate
// advances the key time, steps the key (stopping with the key event at the last
// key, else wrapping) and lerps/slerps between the two keys (through an
// inferred inline LerpTo), applying the key offset and rotation for delta
// motion, then sets the velocity and target rotation. CStaticObject,
// CMatrix, CQuaternion, CLightVolumeManager and CDrawContext are named by
// the mangled symbols;
// CStaticObject declares its virtual functions in the order of
// __vt__13CStaticObject (placeholders named by offset for the slots not used
// here), its members, the flag names and the mesh, key, extent and listener
// views are inferred, and the CVector3 view copies through a double pair as
// in player_weapon.cpp. The rest of the file is not part of this unit.
extern "C" {
#include <math.h>
}
#undef fabs
inline double fabs(double x) { return __fabs(x); }

void MathFunRotateAboutZ(float*, float*, float);
float MathFunAtan2F(float, float);
float MathFunNormalizeAngleNegativePiToPi(float);
float MathFunRandomReal(float, float);
unsigned long GetFrameCount();

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3& operator+=(const CVector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    void PreTranslate(CVector3);
    void RotateX(float);
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void Orthonormalize();
    CVector3 GetRight() const { return right; }
    CVector3 GetFront() const { return forward; }
    CVector3 GetUp() const { return up; }
    void SetPos(CVector3);
    void Translate(CVector3);
    void TransformPoint(CVector3&, CVector3) const;
    void Multiply(const CMatrix&, const CMatrix&);
    void BuildScale(float);
    static void InitClass();
    static bool s_ClassInit;

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);
    CQuaternion() {}
    CQuaternion(float ax, float ay, float az, float aw) : w(aw), x(ax), y(ay), z(az) {}
    void Slerp(CQuaternion, CQuaternion, float);
    void Multiply(CQuaternion);
    void Normalize();
    void Rotate(CVector3&, CVector3);
    void Set(float, float, float, float);
    void GetMatrix(CMatrix&) const;

    float w;
    float x;
    float y;
    float z;
};

class BPDLightVolume;

class CLightVolumeManager {
public:
    BPDLightVolume* GetVolume();
    void Update(float);

    unsigned char unknown00[4];
};

/* inferred: the draw context's transform pointer at +192 */
class CDrawContext {
public:
    unsigned char unknown000[192];
    CMatrix* m_localToWorld;
};

/* inferred: the object's mesh (+48), drawn through its slot +12 */
class MeshView {
public:
    virtual void unknown08();
    virtual void Draw(CDrawContext&, int);
    virtual void unknown10();
    virtual bool IsLit();
};

/* inferred: a motion key (32 bytes): position and rotation */
struct MotionKey {
    CVector3 m_position;
    CQuaternion m_rotation;
};

/* inferred: the other matrix listener, told through its slot +16 */
class MatrixListenerView {
public:
    virtual void unknown08();
    virtual void unknown0c();
    virtual void Update(void*, const CMatrix&);
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void GetExtents(CVector3&, CVector3&) const;
};

/* inferred: a node of the scene's extent lists, its value at +8 */
struct ExtentNodeView {
    unsigned char unknown00[8];
    float m_value;
};

class CStaticObject {
public:
    virtual void MarkForDestruction(int);
    virtual ~CStaticObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const void*);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual void GetLocalBoundingVolume(int) const;
    virtual void GetWorldBoundingVolume(int) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual void vf060();
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void vf068();
    virtual void vf06c();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf08c();
    virtual void vf090();
    virtual void vf094();
    virtual void vf098();
    virtual void vf09c();
    virtual void vf0a0();
    virtual void vf0a4();
    virtual void vf0a8();
    virtual void vf0ac();
    virtual void vf0b0();
    virtual void vf0b4();
    virtual void vf0b8();
    virtual void vf0bc();
    virtual void vf0c0();
    virtual void vf0c4();
    virtual void vf0c8();
    virtual void vf0cc();
    virtual void vf0d0();
    virtual void vf0d4();
    virtual void vf0d8();
    virtual void vf0dc();
    virtual void vf0e0();
    virtual void vf0e4();
    virtual void vf0e8();
    virtual void vf0ec();
    virtual void vf0f0();
    virtual void vf0f4();
    virtual void vf0f8();
    virtual void vf0fc();
    virtual void vf100();
    virtual void vf104();
    virtual void vf108();
    virtual void vf10c();
    virtual void vf110();
    virtual void vf114();
    virtual void vf118();
    virtual void vf11c();
    virtual void vf120();
    virtual void vf124();
    virtual void vf128();
    virtual void vf12c();
    virtual void vf130();
    virtual void vf134();
    virtual void vf138();
    virtual void vf13c();
    virtual void vf140();
    virtual void vf144();
    virtual void vf148();
    virtual void AttemptScriptMotionUpdate(float);
    virtual void AttemptAnimMotionUpdate(float);
    virtual void AttemptPathMotionUpdate(float);
    virtual void CommitScriptMotionUpdate();
    virtual void CommitAnimMotionUpdate();
    virtual void CommitPathMotionUpdate();

    bool IsFollowingPath() const { return m_pathState != 0 && m_pathState != 1; }
    CVector3 PositionRow() const { return m_tm.position; }
    void AnimUpdate(float);
    void BeginAnimMotionUpdate(float);
    /* inferred: no out-of-line copy survives; BeginUpdate inlines it */
    void BeginScriptMotionUpdate(float) {
        m_pathPosition = PositionRow();
        if (m_scriptMove && m_time1b8 <= 0.0f) {
            TriggerScriptEvent(m_moveEvent, 0, false);
            m_time1b8 = 0.0f;
            m_scriptMove = false;
            m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
        }
        if (m_scriptRotate) {
            m_scriptRotation.Normalize();
            if (m_time1bc <= 0.0f) {
                TriggerScriptEvent(m_turnEvent, 0, false);
                m_time1bc = 0.0f;
                m_scriptRotate = false;
                m_scriptTurn.Set(0.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        if (m_scriptValue && m_time1c0 <= 0.0f) {
            TriggerScriptEvent(m_valueEvent, 0, false);
            m_time1c0 = 0.0f;
            m_scriptValue = false;
            m_value1b0 = 0.0f;
        }
        m_scriptHold = false;
    }
    /* defined out of line earlier in static_obj.cpp (static_obj_pathbegin.cpp);
       BeginUpdate inlines it */
    void BeginPathMotionUpdate(float) {
        m_pathPosition = PositionRow();
        m_pathSpeed = m_speed216;
        m_scriptMove = true;
        UpdatePathMotionDestAndSpeed();
    }
    void UpdatePathMotionDestAndSpeed();


    unsigned char unknown004[8];
    ExtentNodeView* m_extentMinX;
    ExtentNodeView* m_extentMaxX;
    ExtentNodeView* m_extentMinY;
    ExtentNodeView* m_extentMaxY;
    unsigned char unknown01c[20];
    MeshView* m_mesh;
    unsigned char unknown034[12];
    CMatrix m_tm;
    unsigned char unknown080[88];
    float m_speed216;
    unsigned char unknown0dc[4];
    float m_maxSpeed;
    float m_accel;
    float m_turnSlowdown;
    unsigned char unknown0ec[8];
    float m_yawRate;
    float m_pitchRate;
    float m_maxTurnRate;
    float m_turnAccel;
    int m_pathState;
    unsigned char unknown108[4];
    void* m_listenerData;
    MatrixListenerView* m_matrixListener;
    MatrixListenerView* m_listener;
    unsigned char unknown118[8];
    CVector3 m_animPosition;
    CVector3 m_keyOffset;
    CQuaternion m_keyRotation;
    CQuaternion m_animRotation;
    CVector3 m_velocity;
    CVector3 m_pathPosition;
    CQuaternion m_scriptRotation;
    CQuaternion m_scriptTurn;
    CQuaternion m_scriptTargetRotation;
    float m_value1b0;
    float m_value1b4;
    float m_time1b8;
    float m_time1bc;
    float m_time1c0;
    int m_moveEvent;
    int m_turnEvent;
    int m_valueEvent;
    float m_step1d0;
    float m_step1d4;
    float m_step1d8;
    unsigned char unknown1dc[4];
    unsigned char m_flag80 : 1;
    unsigned char m_scriptMove : 1;
    unsigned char m_scriptRotate : 1;
    unsigned char m_scriptValue : 1;
    unsigned char m_transformDelta : 1;
    unsigned char m_animMove : 1;
    unsigned char m_flags03 : 2;
    unsigned char m_noSteer : 1;
    unsigned char m_flags1e1a : 1;
    unsigned char m_scriptHold : 1;
    unsigned char m_flags1e1b : 5;
    unsigned char unknown1e2[6];
    CVector3 m_pathDest;
    float m_pathSpeed;
    unsigned char unknown1fc[24];
    int m_drawArg;
    int m_keyEvent;
    unsigned char unknown21c[4];
    int m_lastKey;
    int m_keyCount;
    float m_keyTime;
    int m_keyFrames;
    int m_key;
    float m_rotationRate;
    CQuaternion m_targetRotation;
    MotionKey* m_keys;
    void* m_attached;
    CVector3 m_attachOffset;
    CLightVolumeManager m_lightVolumes;
};


void CStaticObject::AttemptPathMotionUpdate(float dt) {
    CVector3 dir;
    dir.x = m_pathDest.x - m_pathPosition.x;
    dir.y = m_pathDest.y - m_pathPosition.y;
    dir.z = m_pathDest.z - m_pathPosition.z;
    float length = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
    if (length != 0.0f) {
        float inv = 1.0f / length;
        dir.x *= inv;
        dir.y *= inv;
        dir.z *= inv;
    }
    if (!m_noSteer) {
        CVector3 right;
        CVector3 v;
        CVector3 up;
        v = m_tm.GetFront();
        float yaw = MathFunAtan2F(v.x, v.y);
        MathFunRotateAboutZ(&v.x, &v.y, -yaw);
        float pitch = MathFunAtan2F(-v.z, v.y);
        v = dir;
        float targetYaw = MathFunAtan2F(v.x, v.y);
        MathFunRotateAboutZ(&v.x, &v.y, -targetYaw);
        float targetPitch = MathFunAtan2F(-v.z, v.y);
        float turn = MathFunNormalizeAngleNegativePiToPi(targetYaw - yaw);
        if (turn < -0.008726646f) {
            if (m_yawRate > 0.0f) {
                if ((float)(fabs(m_yawRate / m_turnAccel) + 2.0) >= (float)fabs(turn / m_yawRate)) {
                    m_yawRate -= m_turnAccel;
                } else {
                    m_yawRate += m_turnAccel;
                    if (m_yawRate > m_maxTurnRate)
                        m_yawRate = m_maxTurnRate;
                }
            } else {
                m_yawRate += m_turnAccel;
                if (m_yawRate > m_maxTurnRate)
                    m_yawRate = m_maxTurnRate;
            }
        } else if (turn > 0.008726646f) {
            if (m_yawRate < 0.0f) {
                if ((float)(fabs(m_yawRate / m_turnAccel) + 2.0) >= (float)fabs(turn / m_yawRate)) {
                    m_yawRate += m_turnAccel;
                } else {
                    m_yawRate -= m_turnAccel;
                    if (m_yawRate < -m_maxTurnRate)
                        m_yawRate = -m_maxTurnRate;
                }
            } else {
                m_yawRate -= m_turnAccel;
                if (m_yawRate < -m_maxTurnRate)
                    m_yawRate = -m_maxTurnRate;
            }
        } else {
            m_yawRate = 0.6f * m_yawRate;
        }
        turn = MathFunNormalizeAngleNegativePiToPi(targetPitch - pitch);
        if (turn < -0.008726646f) {
            if (m_pitchRate > 0.0f) {
                if ((float)(fabs(m_pitchRate / m_turnAccel) + 2.0) >= (float)fabs(turn / m_pitchRate)) {
                    m_pitchRate -= m_turnAccel;
                } else {
                    m_pitchRate += m_turnAccel;
                    if (m_pitchRate > m_maxTurnRate)
                        m_pitchRate = m_maxTurnRate;
                }
            } else {
                m_pitchRate += m_turnAccel;
                if (m_pitchRate > m_maxTurnRate)
                    m_pitchRate = m_maxTurnRate;
            }
        } else if (turn > 0.008726646f) {
            if (m_pitchRate < 0.0f) {
                if ((float)(fabs(m_pitchRate / m_turnAccel) + 2.0) >= (float)fabs(turn / m_pitchRate)) {
                    m_pitchRate += m_turnAccel;
                } else {
                    m_pitchRate -= m_turnAccel;
                    if (m_pitchRate < -m_maxTurnRate)
                        m_pitchRate = -m_maxTurnRate;
                }
            } else {
                m_pitchRate -= m_turnAccel;
                if (m_pitchRate < -m_maxTurnRate)
                    m_pitchRate = -m_maxTurnRate;
            }
        } else {
            m_pitchRate = 0.6f * m_pitchRate;
        }
        m_tm.RotateX(m_pitchRate);
        right = m_tm.GetRight();
        v = m_tm.GetFront();
        up = m_tm.GetUp();
        MathFunRotateAboutZ(&right.x, &right.y, -m_yawRate);
        MathFunRotateAboutZ(&v.x, &v.y, -m_yawRate);
        MathFunRotateAboutZ(&up.x, &up.y, -m_yawRate);
        m_tm.SetRight(right);
        m_tm.SetFront(v);
        m_tm.SetUp(up);
        if (GetFrameCount() & 0x20)
            m_tm.Orthonormalize();
    }
    float speed;
    if (m_turnSlowdown != 0.0f) {
        speed = m_maxSpeed * (1.0 - m_turnSlowdown / m_maxTurnRate * fabs(m_yawRate));
    } else
        speed = m_maxSpeed;
    float limit = MathFunRandomReal(0.8f * speed, speed);
    if (m_speed216 > limit) {
        m_speed216 = m_pathSpeed - 2.0f * m_accel * dt;
        if (m_speed216 < limit)
            m_speed216 = limit;
    } else {
        m_speed216 = m_accel * dt + m_pathSpeed;
        if (m_speed216 > limit)
            m_speed216 = limit;
    }
    if (m_noSteer) {
        float step = m_speed216 * dt;
        dir.x *= step;
        dir.y *= step;
        dir.z *= step;
    } else {
        float step = m_speed216 * dt;
        const CVector3& front = m_tm.GetFront();
        dir.x = step * front.x;
        dir.y = step * front.y;
        dir.z = step * front.z;
    }
    dir += m_pathPosition;
    m_tm.SetPos(dir);
    if (m_listener) {
        m_listener->Update(m_listenerData, m_tm);
        CVector3 min;
        CVector3 max;
        m_listener->GetExtents(min, max);
        m_extentMinX->m_value = min.x;
        m_extentMinY->m_value = min.y;
        m_extentMaxX->m_value = max.x;
        m_extentMaxY->m_value = max.y;
    } else {
        CVector3 position;
        GetPosition(position);
        m_extentMinX->m_value = position.x;
        m_extentMinY->m_value = position.y;
        m_extentMaxX->m_value = position.x;
        m_extentMaxY->m_value = position.y;
    }
}

void CStaticObject::AttemptAnimMotionUpdate(float dt) {
    if (m_keyCount && m_keyFrames) {
        CVector3 delta;
        delta.x = dt * m_velocity.x;
        delta.y = dt * m_velocity.y;
        delta.z = dt * m_velocity.z;
        CQuaternion rotation;
        rotation.Slerp(m_targetRotation, m_animRotation, dt * m_rotationRate);
        rotation.GetMatrix(m_tm);
        m_tm.SetPos(m_animPosition);
        m_tm.Translate(delta);
    }
    if (m_listener) {
        m_listener->Update(m_listenerData, m_tm);
        CVector3 min;
        CVector3 max;
        m_listener->GetExtents(min, max);
        m_extentMinX->m_value = min.x;
        m_extentMinY->m_value = min.y;
        m_extentMaxX->m_value = max.x;
        m_extentMaxY->m_value = max.y;
    } else {
        CVector3 position;
        GetPosition(position);
        m_extentMinX->m_value = position.x;
        m_extentMinY->m_value = position.y;
        m_extentMaxX->m_value = position.x;
        m_extentMaxY->m_value = position.y;
    }
}

void CStaticObject::AttemptScriptMotionUpdate(float dt) {
    float moveStep = dt;
    float turnStep = dt;
    float valueStep = dt;
    float value = m_value1b4;
    CVector3 delta;
    delta.x = delta.y = delta.z = 0.0f;
    if (m_scriptMove) {
        moveStep = m_time1b8 < dt ? m_time1b8 : dt;
        delta.x = moveStep * m_velocity.x;
        delta.y = moveStep * m_velocity.y;
        delta.z = moveStep * m_velocity.z;
    }
    if (m_scriptRotate) {
        turnStep = m_time1bc < dt ? m_time1bc : dt;
        if (turnStep < 1.0f) {
            CQuaternion identity(0.0f, 0.0f, 0.0f, 1.0f);
            m_scriptTargetRotation.Slerp(m_scriptTurn, identity, turnStep);
        } else {
            m_scriptTargetRotation = m_scriptTurn;
        }
        m_scriptTargetRotation.Multiply(m_scriptRotation);
    } else {
        m_scriptTargetRotation = m_scriptRotation;
    }
    if (m_scriptValue) {
        valueStep = m_time1c0 < dt ? m_time1c0 : dt;
        value += m_value1b0 * valueStep;
    }
    m_scriptTargetRotation.GetMatrix(m_tm);
    m_tm.SetPos(m_pathPosition);
    m_tm.TransformPoint(delta, delta);
    m_tm.SetPos(delta);
    CMatrix scale;
    scale.BuildScale(value);
    m_tm.Multiply(scale, m_tm);
    m_step1d0 = moveStep;
    m_step1d4 = turnStep;
    m_step1d8 = valueStep;
    if (m_listener) {
        m_listener->Update(m_listenerData, m_tm);
        CVector3 min;
        CVector3 max;
        m_listener->GetExtents(min, max);
        m_extentMinX->m_value = min.x;
        m_extentMinY->m_value = min.y;
        m_extentMaxX->m_value = max.x;
        m_extentMaxY->m_value = max.y;
    } else {
        CVector3 position;
        GetPosition(position);
        m_extentMinX->m_value = position.x;
        m_extentMinY->m_value = position.y;
        m_extentMaxX->m_value = position.x;
        m_extentMaxY->m_value = position.y;
    }
}

void CStaticObject::BeginUpdate(float dt) {
    m_lightVolumes.Update(dt);
    AnimUpdate(dt);
    if (IsFollowingPath()) {
        BeginPathMotionUpdate(dt);
    } else if (m_scriptMove || m_scriptRotate || m_scriptValue) {
        BeginScriptMotionUpdate(dt);
    } else if (m_animMove) {
        BeginAnimMotionUpdate(dt);
    } else if (m_listener) {
        CVector3 min;
        CVector3 max;
        m_listener->GetExtents(min, max);
        m_extentMinX->m_value = min.x - 0.015625f;
        m_extentMinY->m_value = min.y - 0.015625f;
        m_extentMaxX->m_value = 0.015625f + max.x;
        m_extentMaxY->m_value = 0.015625f + max.y;
    } else {
        CVector3 position;
        GetPosition(position);
        m_extentMinX->m_value = position.x - 0.015625f;
        m_extentMinY->m_value = position.y - 0.015625f;
        m_extentMaxX->m_value = 0.015625f + position.x;
        m_extentMaxY->m_value = 0.015625f + position.y;
    }
}

/* inferred inline helper */
inline void LerpTo(CVector3& r, const CVector3& a, const CVector3& b, float t) {
    float fx, fy, fz;
    fx = a.x;
    fy = a.y;
    fz = a.z;
    r.x = fx + t * (b.x - fx);
    r.y = fy + t * (b.y - fy);
    r.z = fz + t * (b.z - fz);
}

void CStaticObject::BeginAnimMotionUpdate(float dt) {
    if (m_keyCount && m_keyFrames) {
        m_animPosition = PositionRow();
        m_animRotation.SetFromMatrix(m_tm);
        m_keyTime += dt;
        while (m_keyTime >= (float)m_keyFrames) {
            m_keyTime -= (float)m_keyFrames;
            m_key++;
        }
        int next = m_key + 1;
        if (m_lastKey) {
            if (next >= m_lastKey)
                next = m_lastKey - 1;
            if (m_key >= m_lastKey) {
                if (m_keyEvent != -1) {
                    TriggerScriptEvent(m_keyEvent, 0, false);
                    m_keyEvent = -1;
                }
                m_key = m_lastKey - 1;
                m_animMove = false;
            }
        } else {
            m_key %= m_keyCount;
            next %= m_keyCount;
        }
        CVector3 from = m_keys[m_key].m_position;
        CVector3 to = m_keys[next].m_position;
        float t = m_keyTime / (float)m_keyFrames;
        if (t < 0.0f)
            t = 0.0f;
        if (t > 1.0f)
            t = 1.0f;
        CVector3 pos;
        LerpTo(pos, from, to, t);
        CQuaternion current;
        current = m_keys[m_key].m_rotation;
        CQuaternion target;
        target = m_keys[next].m_rotation;
        CQuaternion rotation;
        rotation.Slerp(target, current, t);
        if (m_transformDelta) {
            m_keyRotation.Rotate(pos, pos);
            pos.x += m_keyOffset.x;
            pos.y += m_keyOffset.y;
            pos.z += m_keyOffset.z;
            rotation.Multiply(m_keyRotation);
        }
        m_velocity.x = pos.x - m_animPosition.x;
        float rate = 1.0f / dt;
        m_velocity.y = pos.y - m_animPosition.y;
        m_velocity.z = pos.z - m_animPosition.z;
        m_targetRotation = rotation;
        m_velocity.x *= rate;
        m_velocity.y *= rate;
        m_velocity.z *= rate;
        m_rotationRate = rate;
    }
}
