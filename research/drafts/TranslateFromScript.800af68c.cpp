// A fragment of static_obj.cpp (0x800af7c0): CStaticObject's Stop- and
// StartMotionPlayback (clamps the key range, saves the starting rotation and
// position and, unless a flag defers it, jumps to the first key), PlayAnimation
// (clamps the frame range and records speed, range and event, reversing for a
// negative speed over a descending range), the light-volume forwards to its
// manager (+608), SetBasis (sets the transform's rows and refreshes the
// rotation at +384), SetPosition and SetTMLocalToWorld (also refreshing the
// rotation), forwarding to the object's transform
// (a CMatrix at +64). The animation fields and flag bits are inferred. The file
// name is this project's; the original record is static_obj.cpp and the functions
// around these are not reconstructed. CStaticObject and CMatrix's methods are named by
// the mangled symbols; CStaticObject is an inferred non-virtual view with only the
// transform declared, the bodies are inferred from the calls, and the by-value
// row getters are inferred inline helpers.
//
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: vectors are
// copied as two lfd/stfd pairs, which an implicit copy through the double pair
// reproduces.
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);
    void Orthonormalize();
    void RotateX(float);
    void RotateY(float);
    void RotateZ(float);
    void Rotate(CVector3, float);
    void Translate(CVector3);
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);
    void Multiply(const CMatrix&, const CMatrix&);
    void Ident();

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

struct BPDLightVolume;


enum EBSEventEnum {};

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

// A motion key (inferred): position, then rotation.
struct StaticMotionKey {
    CVector3 position;
    CQuaternion rotation;
};

class CLightVolumeManager {
public:
    BPDLightVolume* GetVolume();
    void RemoveVolume(BPDLightVolume*);
    void AddVolume(BPDLightVolume*);
};

class CStaticObject {
public:
    void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum);
    void StopMotionPlayback();
    void StartMotionPlayback(int, int, EBSEventEnum);
    void PlayAnimation(int, int, int, EBSEventEnum);
    BPDLightVolume* GetLightVolume();
    void ExitLightVolume(BPDLightVolume*);
    void EnterLightVolume(BPDLightVolume*);
    void SetBasis(CVector3, CVector3, CVector3);
    void SetPosition(CVector3);
    void SetTMLocalToWorld(const CMatrix&);

    CVector3 UpRow() const { return m_tm.up; }
    CVector3 ForwardRow() const { return m_tm.forward; }
    CVector3 RightRow() const { return m_tm.right; }
    CVector3 PositionRow() const { return m_tm.position; }

    unsigned char unknown00[64];
    CMatrix m_tm;
    unsigned char unknown080[160];
    CVector3 m_motionPosition;
    CVector3 m_savedPosition;
    CQuaternion m_motionStart;
    CQuaternion m_motionRotation;
    CVector3 m_translateVelocity;
    unsigned char unknown170[16];
    CQuaternion m_rotation;
    unsigned char unknown190[40];
    float m_translateTime;
    unsigned char unknown1bc[8];
    EBSEventEnum m_translateEvent;
    unsigned char unknown1c8[24];
    unsigned char m_flag80 : 1;
    unsigned char m_flag40 : 1;
    unsigned char m_flag20 : 1;
    unsigned char m_flag10 : 1;
    unsigned char m_flag08 : 1;
    unsigned char m_flag04 : 1;
    unsigned char m_reverse : 1;
    unsigned char m_flag01 : 1;
    unsigned char unknown1e1[27];
    EBSEventEnum m_event;
    int m_start;
    int m_end;
    int m_frameCount;
    unsigned char unknown20c[4];
    int m_speed;
    int m_frame;
    EBSEventEnum m_motionEvent;
    int m_motionFrame;
    int m_motionEnd;
    int m_motionKeyCount;
    unsigned char unknown228[8];
    int m_motionKey;
    unsigned char unknown234[20];
    StaticMotionKey* m_motionKeys;
    unsigned char unknown24c[20];
    CLightVolumeManager m_lightVolumes;
};

// Inferred inline: whether the first time comes first (a zero second time
// does not count, nor does a first time that is not positive).
static inline bool IsSooner(float a, float b) {
    return (a < b && a > 0.0f) || b == 0.0f;
}

void CStaticObject::TranslateFromScript(CVector3& velocity, CVector3& distance, EBSEventEnum event) {
    m_flag40 = 1;
    m_translateEvent = event;
    if (distance.x == 0.0f && distance.y == 0.0f && distance.z == 0.0f) {
        m_translateTime = 3.4028235e38f;
    } else {
        CVector3 time;
        time.z = 0.0f;
        time.y = 0.0f;
        time.x = 0.0f;
        time.z = (float)(double)time.z;
        if (velocity.x != 0.0f && distance.x != 0.0f)
            time.x = distance.x / velocity.x;
        if (velocity.y != 0.0f && distance.y != 0.0f)
            time.y = distance.y / velocity.y;
        if (velocity.z != 0.0f && distance.z != 0.0f)
            time.z = distance.z / velocity.z;
        time.x = IsSooner(time.x, time.y) ? time.x : time.y;
        time.x = IsSooner(time.x, time.z) ? time.x : time.z;
        m_translateTime = time.x;
    }
    m_translateVelocity = velocity;
}

void CStaticObject::StopMotionPlayback() {
    m_flag04 = 0;
}

void CStaticObject::StartMotionPlayback(int frame, int end, EBSEventEnum event) {
    if (!m_motionKeys)
        return;
    if (frame < 0)
        frame = 0;
    if (frame >= m_motionKeyCount)
        frame = m_motionKeyCount - 1;
    if (end < 0)
        end = 0;
    if (end >= m_motionKeyCount)
        end = m_motionKeyCount;
    m_motionFrame = frame;
    m_motionKey = frame;
    m_motionEnd = end;
    m_motionEvent = event;
    m_motionStart.SetFromMatrix(m_tm);
    m_savedPosition = PositionRow();
    if (!m_flag08) {
        m_motionRotation = m_motionKeys[m_motionKey].rotation;
        m_motionPosition = m_motionKeys[m_motionKey].position;
        m_motionRotation.GetMatrix(m_tm);
        m_tm.SetPos(m_motionPosition);
    }
    m_flag04 = 1;
}

void CStaticObject::PlayAnimation(int speed, int start, int end, EBSEventEnum event) {
    if (speed < 0 && start > end) {
        m_reverse = 1;
        speed = -speed;
    } else if (speed < 0) {
        m_reverse = 0;
        speed = 0;
    } else {
        m_reverse = 0;
    }
    if (start < 0)
        start = 0;
    if (start >= m_frameCount)
        start = m_frameCount - 1;
    if (end < 0)
        end = 0;
    if (end >= m_frameCount)
        end = m_frameCount;
    m_speed = speed;
    m_start = start;
    m_frame = start;
    m_end = end;
    m_event = event;
}

BPDLightVolume* CStaticObject::GetLightVolume() {
    return m_lightVolumes.GetVolume();
}

void CStaticObject::ExitLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.RemoveVolume(volume);
}

void CStaticObject::EnterLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.AddVolume(volume);
}

void CStaticObject::SetBasis(CVector3 right, CVector3 front, CVector3 up) {
    m_tm.SetRight(right);
    m_tm.SetFront(front);
    m_tm.SetUp(up);
    m_rotation.SetFromMatrix(m_tm);
}

void CStaticObject::SetPosition(CVector3 position) {
    m_tm.SetPos(position);
}

void CStaticObject::SetTMLocalToWorld(const CMatrix& tm) {
    m_tm = tm;
    m_rotation.SetFromMatrix(tm);
}
