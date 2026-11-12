// A fragment of hierobject.cpp (0x8009988c): CHierObject::Draw (draws itself
// as a static object, then each child through its virtual Draw),
// SetInheritedMatrix, GetWorldBoundingVolume (the root's CSG volume or the
// object's own volume for the collision volume types) and GetSubVolume (the
// volume of the indexed sub-object, recursing from the root; the compiler
// inlines the recursion). The classes are named by the mangled symbols; the
// virtual functions follow ISceneNode's virtual table, and the members and
// flag-byte bit-field view are inferred from offsets.
class CDrawContext;
class CCollision;
class IVolume;

#include "game/CMatrix.h"


class CCSGVolume {
public:
    unsigned char unknown00[12];
};

class ISceneNode {
public:
    enum EVolumeType {};

    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
};

class CStaticObject : public ISceneNode {
public:
    virtual ~CStaticObject();
    void Draw(CDrawContext&);
};

class CHierObject : public CStaticObject {
public:
    virtual ~CHierObject();
    void Draw(CDrawContext&);
    void SetInheritedMatrix(CMatrix&);
    IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    IVolume* GetSubVolume(int) const;

    unsigned char unknown004[272];
    IVolume* m_volume;
    unsigned char unknown118[1128];
    CHierObject* m_firstChild;
    CHierObject* m_nextSibling;
    bool m_paused : 1;
    unsigned char unknown588b6 : 1;
    bool m_isRoot : 1;
    unsigned char unknown588b4 : 5;
    unsigned char unknown589[11];
    CCSGVolume m_csgVolume;
    CHierObject* m_subObjects[20];
    CMatrix m_inheritedMatrix;
};

void CHierObject::Draw(CDrawContext& context) {
    CStaticObject::Draw(context);
    for (CHierObject* child = m_firstChild; child; child = child->m_nextSibling)
        child->Draw(context);
}

void CHierObject::SetInheritedMatrix(CMatrix& matrix) {
    m_inheritedMatrix = matrix;
}

IVolume* CHierObject::GetWorldBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        if (m_isRoot)
            return (IVolume*)&m_csgVolume;
        return m_volume;
    case 0:
    case 1:
    case 2:
        if (m_isRoot)
            return (IVolume*)&m_csgVolume;
        return m_volume;
    case 4:
        return 0;
    }
    return 0;
}

IVolume* CHierObject::GetSubVolume(int index) const {
    if (m_isRoot && index > 0) {
        if (m_subObjects[index])
            return m_subObjects[index]->GetSubVolume(index);
        return 0;
    }
    return m_volume;
}
