extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) { d.v[0] = x; d.v[1] = y; d.v[2] = z; }
    CVector3(const CVector3& o) { d.pair[0] = o.d.pair[0]; d.pair[1] = o.d.pair[1]; }
    void Cross(const CVector3& a, const CVector3& b) {
        d.v[0] = a.d.v[1] * b.d.v[2] - a.d.v[2] * b.d.v[1];
        d.v[1] = a.d.v[2] * b.d.v[0] - a.d.v[0] * b.d.v[2];
        d.v[2] = a.d.v[0] * b.d.v[1] - a.d.v[1] * b.d.v[0];
    }
    float Length() const { return sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]); }
    float Normalize() {
        float length = Length();
        if (length != 0.0f) {
            d.v[0] *= 1.0f / length;
            d.v[1] *= 1.0f / length;
            d.v[2] *= 1.0f / length;
        }
        return length;
    }

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void SetPos(CVector3);
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void TransformPoint(CVector3&, CVector3) const;
    const CVector3& GetRow(int i) const { return *(const CVector3*)m[i]; }
    CVector3 GetPos() const { return GetRow(3); }

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CLine3 {
public:
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    ~CLine3() {}
    void SetSE(CVector3 start, CVector3 end) {
        m_start = start;
        m_end = end;
        m_dir.d.v[0] = m_end.d.v[0] - m_start.d.v[0];
        m_dir.d.v[1] = m_end.d.v[1] - m_start.d.v[1];
        m_dir.d.v[2] = m_end.d.v[2] - m_start.d.v[2];
        m_flag38 = 0;
        m_flag39 = 0;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
} __attribute__((aligned(16)));

class CCollision {
public:
    CCollision();
    ~CCollision();

    unsigned char unknown00[12];
    float m_t;
    unsigned char m_flag10 : 1;
    unsigned char unknown11[7];
    CVector3* m_normal;
    unsigned char unknown1c[4];
};

class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;
class CVolFiber;
class CAnimatedVolume;
class CWorldVolume;

/* IVolume's virtuals in table order up to the line test. */
class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
};

class CVolSphere {
public:
    CVector3 GetCenter() const;

    unsigned char unknown00[32];
};

class CPlayerObject;
class CSoldierObject;

class ISceneNode {
public:
    virtual void unknown008();
    virtual void unknown012();
    virtual void unknown016();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown032();
    virtual void unknown036();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown052();
    virtual IVolume* GetWorldBoundingVolume(int) const;
    virtual void unknown060();
    virtual void GetPosition(CVector3&) const;
    virtual void unknown068();
    virtual void unknown072();
    virtual void unknown076();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown092();
    virtual void unknown096();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown112();
    virtual void unknown116();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown132();
    virtual void unknown136();
    virtual void unknown140();
    virtual void unknown144();
    virtual CSoldierObject* AsSoldierObject();
    virtual void unknown152();
    virtual CPlayerObject* AsPlayerObject();
};

class CPlayerObject {
public:
    bool IsPlayerCrouching() const;

    unsigned char unknown000[444];
    float m_eyeHeight;
    float GetEyeHeight() const { return m_eyeHeight; }
};

class CSoldierObject {
public:
    void GetLookMatrix(CMatrix&);
};

class CScene {
public:
    unsigned char unknown000[24];
    ISceneNode* m_worldObject;
};

extern CScene g_scene;

enum EScriptBulletType {};

namespace dwi {
template <class T>
class IVisitor {
public:
    virtual ~IVisitor() {}
    virtual void Visit(T&) = 0;
};
}

class CExplosion : public dwi::IVisitor<ISceneNode> {
public:
    virtual void Visit(ISceneNode&);
    virtual ~CExplosion();
    bool CheckForObstructions(ISceneNode&);

    unsigned char unknown04[12];
    CVolSphere m_sphere;
    float data30;
    float data34;
    EScriptBulletType m_type;
    ISceneNode* m_source;
    bool data40;
};

/* inferred: tests the line from the explosion side (end offset turned into the
   explosion's frame, from the center) to the target (start offset, from the
   target's position) against the world (1: nothing hit) */
inline int TestLine(const CMatrix& frame, const CVector3& start, const CVector3& end, const CVector3& position,
                        const CVolSphere& sphere) {
    CVector3 center(sphere.GetCenter());
    CVector3 target;
    frame.TransformPoint(target, start);
    CVector3 source;
    frame.TransformPoint(source, end);
    target.d.v[0] += position.d.v[0];
    target.d.v[1] += position.d.v[1];
    target.d.v[2] += position.d.v[2];
    source.d.v[0] += center.d.v[0];
    source.d.v[1] += center.d.v[1];
    source.d.v[2] += center.d.v[2];
    CLine3 line(source, target);
    CCollision collision;
    collision.m_flag10 = 0;
    return g_scene.m_worldObject->GetWorldBoundingVolume(0)->TestCollision(line, collision, false);
}

/* inferred: is the explosion more than 2 units above the node (only for
   bullet types 21 to 23) */
inline bool IsAbove(const CExplosion& explosion, ISceneNode& node) {
    bool above;
    switch (explosion.m_type) {
    case 21:
    case 22:
    case 23: {
        CVector3 nodePosition;
        node.GetPosition(nodePosition);
        CVector3 center = explosion.m_sphere.GetCenter();
        if (center.d.v[2] - nodePosition.d.v[2] > 2.0f)
            return true;
        above = false;
        break;
    }
    default:
        return false;
    }
    return above;
}

bool CExplosion::CheckForObstructions(ISceneNode& node) {
    CVector3 center = m_sphere.GetCenter();
    CVector3 front;
    CVector3 position;
    node.GetPosition(position);
    front.d.v[0] = position.d.v[0] - center.d.v[0];
    front.d.v[1] = position.d.v[1] - center.d.v[1];
    front.d.v[2] = position.d.v[2] - center.d.v[2];
    front.d.v[2] = 0.0f;
    float distance = front.Normalize();
    if (distance < 0.001f)
        return false;
    CVector3 right;
    right.Cross(front, CVector3(0.0f, 0.0f, 1.0f));
    CMatrix frame;
    frame.SetPos(CVector3(0.0f, 0.0f, 0.0f));
    frame.SetRight(right);
    frame.SetFront(front);
    CVector3 up;
    up.Cross(right, front);
    frame.SetUp(up);
    CVector3 end;
    CVector3 start;
    float scale = 1.0f;
    if (node.AsPlayerObject()) {
        if (node.AsPlayerObject()->IsPlayerCrouching())
            scale = 0.33f;
        position.d.v[2] = position.d.v[2] - node.AsPlayerObject()->GetEyeHeight();
    } else if (node.AsSoldierObject()) {
        CMatrix look;
        node.AsSoldierObject()->GetLookMatrix(look);
        CVector3 eye;
        eye = look.GetPos();
        float height = eye.d.v[2] - position.d.v[2];
        if (height < 0.0f)
            height = 0.0f;
        scale = height / 1.6f;
    }
    int sign;
    if (!IsAbove(*this, node))
        sign = 1;
    else
        sign = -1;
    float offset = 0.25f;
    float height = 1.6f * scale;
    end.d.v[0] = 0.0f;
    end.d.v[1] = 0.0f;
    end.d.v[2] = sign * height;
    start.d.v[0] = offset;
    start.d.v[1] = 0.0f;
    start.d.v[2] = height;
    if (TestLine(frame, start, end, position, m_sphere) != 1) {
        float height2 = 0.5f * height;
        end.d.v[0] = 0.0f;
        end.d.v[1] = 0.0f;
        end.d.v[2] = sign * height2;
        start.d.v[0] = -offset;
        start.d.v[1] = 0.0f;
        start.d.v[2] = height2;
        if (TestLine(frame, start, end, position, m_sphere) != 1) {
            float height3 = 0.01f * height;
            end.d.v[0] = 0.0f;
            end.d.v[1] = 0.0f;
            start.d.v[0] = 0.0f;
            end.d.v[2] = sign * (0.01f * height);
            start.d.v[1] = 0.0f;
            start.d.v[2] = height3;
            if (TestLine(frame, start, end, position, m_sphere) != 1)
                return true;
        }
    }
    return false;
}
