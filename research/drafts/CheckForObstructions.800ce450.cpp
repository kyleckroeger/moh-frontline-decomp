// real2 (2026-11-12): 25 real lines, unchanged: front.Sub and the line's m_dir.Sub (the original's CVector3 Sub member).


#include "game/CVector3.h"


#include "game/CMatrix.h"


class CLine3 {
public:
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    ~CLine3() {}
    void SetSE(CVector3 start, CVector3 end) {
        m_start = start;
        m_end = end;
        m_dir.Sub(m_end, m_start);
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

#include "game/ISceneNode.h"


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

#include "game/CScene.h"


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
    target.x += position.x;
    target.y += position.y;
    target.z += position.z;
    source.x += center.x;
    source.y += center.y;
    source.z += center.z;
    CLine3 line(source, target);
    CCollision collision;
    collision.m_flag10 = 0;
    return g_scene.m_world->GetWorldBoundingVolume((ISceneNode::EVolumeType)0)->TestCollision(line, collision, false);
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
        if (center.z - nodePosition.z > 2.0f)
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
    front.Sub(position, center);
    front.z = 0.0f;
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
        position.z = position.z - node.AsPlayerObject()->GetEyeHeight();
    } else if (node.AsSoldierObject()) {
        CMatrix look;
        node.AsSoldierObject()->GetLookMatrix(look);
        CVector3 eye;
        eye = look.GetPos();
        float height = eye.z - position.z;
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
    end.x = 0.0f;
    end.y = 0.0f;
    end.z = sign * height;
    start.x = offset;
    start.y = 0.0f;
    start.z = height;
    if (TestLine(frame, start, end, position, m_sphere) != 1) {
        float height2 = 0.5f * height;
        end.x = 0.0f;
        end.y = 0.0f;
        end.z = sign * height2;
        start.x = -offset;
        start.y = 0.0f;
        start.z = height2;
        if (TestLine(frame, start, end, position, m_sphere) != 1) {
            float height3 = 0.01f * height;
            end.x = 0.0f;
            end.y = 0.0f;
            start.x = 0.0f;
            end.z = sign * (0.01f * height);
            start.y = 0.0f;
            start.z = height3;
            if (TestLine(frame, start, end, position, m_sphere) != 1)
                return true;
        }
    }
    return false;
}
