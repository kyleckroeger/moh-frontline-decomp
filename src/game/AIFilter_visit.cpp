// A fragment of AIFilter.cpp (0x8005c570): MechanicCollisionChecker::Visit
// (false for the skipped node; nodes whose collision id is -1, 16 or 22, or
// also 15 or 17 when the second flag is set, are ignored; otherwise the
// node's world volume is tested with the checker's fiber, and a hit keeps the
// nearer scaled distance when a distance is set and returns the first flag).
// The file name is this project's; the original record is AIFilter.cpp, after
// AIFilter_linechecks.cpp. The views are those of AIFilter_linechecks.cpp,
// except that the checker's destructor (weak in the original, inline there)
// is declared without a body, first, here: Visit is the checker's only
// other virtual, so it would otherwise be the key function and this unit
// would emit the checker's global virtual table. The 0.0f constant is an item of
// the file's .sdata2 pool.
class CBullet;
class CCollision;
class CDrawContext;
class CLight;
class CPlayerObject;
class CStaticObject;
class CMatrix;
struct AIDoodadView;



#include "game/CVector3.h"

/* inferred: a 16-byte, 8-aligned position (copied as two doubles) */
class CAIFilterRealPosition : public CVector3 {};

class CLine3 {
public:
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    ~CLine3() {}
    void SetSE(CVector3 start, CVector3 end) {
        m_start = start;
        m_end = end;
        m_dir.x = m_end.x - m_start.x;
        m_dir.y = m_end.y - m_start.y;
        m_dir.z = m_end.z - m_start.z;
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






/* ISceneNode's virtuals in table order (slots 20 to 216). They are the weak
   inline defaults of Moh2.cpp in the original; their bodies are not repeated
   here, so they are declared without one. */
#include "game/ISceneNode.h"


class CVolFiber;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;
class CAnimatedVolume;
class CWorldVolume;

/* IVolume's virtuals in table order up to the fiber test. */
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

class CVolFiber : public IVolume {
public:
    CVolFiber(const CLine3&);
    CVolFiber(const CVolFiber&);
    virtual ~CVolFiber();

    unsigned char unknown04[12];
    CLine3 m_line;
};

class CCollision {
public:
    CCollision();
    ~CCollision();

    unsigned char unknown00[12];
    float m_t;
    bool m_lineTest : 1;
    unsigned char unknown10[15];
};

namespace dwi {
template <class T>
class IVisitor {
public:
    virtual ~IVisitor() {}
    virtual bool Visit(T&) = 0;
};
}

class MechanicCollisionChecker : public dwi::IVisitor<ISceneNode> {
public:
    MechanicCollisionChecker(const CVolFiber& fiber, float distance, ISceneNode* skip, bool flag60, bool flag61)
        : m_fiber(fiber), data60(flag60), data61(flag61), m_distance(distance), m_nearest(distance), m_skip(skip) {}
    virtual ~MechanicCollisionChecker();
    virtual bool Visit(ISceneNode&);

    unsigned char unknown04[12];
    CVolFiber m_fiber;
    bool data60;
    bool data61;
    float m_distance;
    float m_nearest;
    ISceneNode* m_skip;
};

class CScene {
public:
    bool VisitAllStaticObjects(dwi::IVisitor<ISceneNode>&);

    unsigned char unknown000[24];
    ISceneNode* m_worldObject;
};

extern CScene g_scene;

class BSGO_Basic {
    int m_field0;
    int m_field4;
    int m_field8;

public:
    virtual void Destroy();
    virtual void* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

/* inferred: the filter's object (+12) and its script object (+12) */
struct AIFilterOwnerView {
    unsigned char unknown00[12];
    BSGO_Basic* m_script;
};

class CAIFilterObject {
public:
    unsigned char unknown00[12];
    AIFilterOwnerView* m_owner;
};


bool MechanicCollisionChecker::Visit(ISceneNode& node) {
    if (&node == m_skip)
        return false;
    CCollision collision;
    EClsnId id = node.GetCollisionId();
    bool test = false;
    if (id != -1 && id != 16 && id != 22)
        test = true;
    if (data61)
        test &= id != 15 && id != 17;
    if (test) {
        IVolume* volume = (IVolume*)node.GetWorldBoundingVolume((ISceneNode::EVolumeType)0);
        if (volume && volume->TestCollision(m_fiber, collision, false) != 1) {
            if (m_distance > 0.0f) {
                float distance = m_distance * collision.m_t;
                if (distance < m_nearest)
                    m_nearest = distance;
            }
            return data60;
        }
    }
    return false;
}
