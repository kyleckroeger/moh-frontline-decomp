// A fragment of bsbifunc.cpp (0x80032208): AI built-ins that continue (walk
// state 8) or stop (walk state 0) a move-point walk, start a walk to the move
// point held in the script object's script data, and set that move point
// (AISetMovePoint) from the kind of point named by the first argument: the AI
// filter's guess of its target's position (1, 2), a script object's scene node
// position (3), a placed object's position (4, 5), the front row of its
// orientation matrix less its position (6), a scene node's position (7), the
// closest path node to a script vector (8, failing when there is none) or a
// point 1.5 units along its rotated -Y axis (10); the second argument is the
// object or vector and the third the speed passed to
// CAIObject::SetupArbitraryPointUpdate, and the result (whether a point was
// found) replaces the arguments. Each reaches the script object's AI object
// through the scene node's AI doodad and its filter, and pops the built-in's
// arguments. The file name is this project's; the original record is
// bsbifunc.cpp; AISnapToMovePoint before these is 8 instructions off (a
// register swap in its three-float copy). The functions, classes and globals
// are named by the mangled symbols; ISceneNode is declared with its virtual
// functions in the order of __vt__10ISceneNode (GetPosition at +64,
// GetScriptObject at +96, GetAIDoodad at +204) and BSGO_Basic in the order of
// __vt__10BSGO_Basic; the doodad, filter, CAIObject, script object, placed
// object, path node and instance data views are inferred (members at their
// offsets, names not original), as are CVector3's in-place setter (its
// arguments are evaluated right to left, so the source is read z, y, x) and
// its double-pair assignment (the copy is made in two doublewords), and the
// quaternion's four-value constructor. The constants are entries of the
// file's .sdata2 pool.
enum EClsnId {};
enum EMovePointType {};
struct BS_STRUCT_Vector_struct;

struct CVector3Pair {
    double a;
    double b;
};

class CVector3 {
public:
    CVector3& operator=(const CVector3& o) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&o;
        return *this;
    }
    void Set(float x_, float y_, float z_) {
        x = x_;
        y = y_;
        z = z_;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));
class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;
struct BSObjectView;
struct AIDoodadView;

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
    virtual int GetLocalBoundingVolume(EVolumeType) const;
    virtual int GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual BSObjectView* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
    virtual const void* AsPlayerObject() const;
    virtual void* AsPlayerWeaponObject();
    virtual const void* AsPlayerWeaponObject() const;
    virtual void* AsAnimatedPlayerObject();
    virtual const void* AsAnimatedPlayerObject() const;
    virtual CLight* AsLight();
    virtual const CLight* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;
    virtual AIDoodadView* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    float x;
    float y;
    float z;
};

struct MovePointInstanceView {
    unsigned char unknown00[172];
    float x;
    float y;
    float z;
};

// BSGO_Basic, the object at BSObject+12: 12 bytes of members, then its
// virtual table pointer; the virtual functions it needs, in the order of
// __vt__10BSGO_Basic.
class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual MovePointInstanceView* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct BSObjectView {
    unsigned char unknown00[8];
    struct BSObjectDataView* data;
    BSGO_Basic* user;
};

class CAIObject {
public:
    void StartWalkToArbitraryPoint(CAIFilterRealPosition&);
    void SetupArbitraryPointUpdate(EMovePointType, BS_STRUCT_Vector_struct*, float);

    unsigned char unknown000[56];
    CAIFilterRealPosition m_position;
    unsigned char unknown044[8];
    bool m_flag4c;
    unsigned char unknown04d[347];
    unsigned int m_flags;
    unsigned char unknown1ac[108];
    int m_walkState;
    unsigned char unknown21c[172];
    float m_storedX;
    float m_storedY;
    float m_storedZ;
};

struct CAIFilterView {
    unsigned char unknown00[8];
    CAIObject* object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterView* filter;
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

inline CAIObject* GetAIObject(void* object) {
    return ((ISceneNode*)object)->GetAIDoodad()->filter->object;
}

void BIFunc_AISnapToMovePoint(int** stack, void* object);

void BIFunc_AIContinueMovePointWalk(int** stack, void* object) {
    GetAIObject(object)->m_walkState = 8;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIStopMovePointWalk(int** stack, void* object) {
    GetAIObject(object)->m_walkState = 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIStartMovePointWalk(int** stack, void* object) {
    CAIFilterView* filter = ((ISceneNode*)object)->GetAIDoodad()->filter;
    MovePointInstanceView* data = ((ISceneNode*)object)->GetScriptObject()->user->GetScriptData();
    CAIFilterRealPosition point(data->x, data->y, data->z);
    filter->object->StartWalkToArbitraryPoint(point);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}


class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    CVector3 GetFront() const { return front; }
    static void InitClass();
    static bool s_ClassInit;

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    CQuaternion(float w_, float x_, float y_, float z_) : w(w_), x(x_), y(y_), z(z_) {}
    void GetMatrix(CMatrix&) const;
    void Rotate(CVector3&);

    float w;
    float x;
    float y;
    float z;
};

struct PlacementView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
    CQuaternion orientation;
};

struct BSObjectDataView {
    unsigned char unknown00[4];
    PlacementView* placement;
};

struct PathNodeView {
    float x;
    float y;
    float z;
};

struct BSVectorView {
    float x;
    float y;
    float z;
};

class CPathNodeList {
public:
    PathNodeView* GetClosestNode(const CAIFilterRealPosition&, bool) const;
};

class CAIFilterGlobal {
public:
    unsigned char unknown00[16];
    CPathNodeList* m_pathNodes;
    unsigned char unknown14[4];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

class CAIFilterObject {
public:
    void GetTargetGuessPosition(CAIFilterRealPosition&);

    unsigned char unknown00[8];
    CAIObject* object;
};

void BIFunc_AISetMovePoint(int** stack, void* object) {
    CAIFilterObject* filter;
    bool found = false;
    filter = (CAIFilterObject*)((ISceneNode*)object)->GetAIDoodad()->filter;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    EMovePointType type = (EMovePointType)*(*stack - (count - 1));
    CVector3 pos;

    switch (type) {
    case 1:
    case 2:
        filter->GetTargetGuessPosition((CAIFilterRealPosition&)pos);
        found = true;
        break;
    case 3: {
        BSObjectView* target = (BSObjectView*)*(*stack - (count - 2));
        if (target) {
            target->user->GetSceneNode()->GetPosition(pos);
            found = true;
        }
        break;
    }
    case 4:
    case 5: {
        BSObjectView* target = (BSObjectView*)*(*stack - (count - 2));
        if (target) {
            PlacementView* placement;
            found = true;
            placement = target->data->placement;
            pos.Set(placement->x, placement->y, placement->z);
        }
        break;
    }
    case 6: {
        PlacementView* placement = ((BSObjectView*)*(*stack - (count - 2)))->data->placement;
        CMatrix matrix;
        placement->orientation.GetMatrix(matrix);
        found = true;
        pos = matrix.GetFront();
        pos.x -= placement->x;
        pos.y -= placement->y;
        pos.z -= placement->z;
        break;
    }
    case 7:
        ((ISceneNode*)*(*stack - (count - 2)))->GetPosition(pos);
        found = true;
        break;
    case 8: {
        BSVectorView* vector = (BSVectorView*)*(*stack - (count - 2));
        PathNodeView* node;
        pos.x = vector->x;
        pos.y = vector->y;
        pos.z = vector->z;
        node = g_aigAIFilterGlobalObject.m_pathNodes->GetClosestNode((CAIFilterRealPosition&)pos, true);
        if (node) {
            found = true;
            pos.Set(node->x, node->y, node->z);
        } else {
            found = false;
        }
        break;
    }
    case 10: {
        BSObjectView* target = (BSObjectView*)*(*stack - (count - 2));
        if (target) {
            PlacementView* placement = target->data->placement;
            pos.Set(placement->x, placement->y, placement->z);
            CQuaternion orientation(placement->orientation.w, placement->orientation.x, placement->orientation.y,
                                    placement->orientation.z);
            CVector3 direction;
            direction.Set(0.0f, -1.0f, 0.0f);
            orientation.Rotate(direction);
            found = true;
            direction.x *= 1.5f;
            direction.y *= 1.5f;
            direction.z *= 1.5f;
            pos.x += direction.x;
            pos.y += direction.y;
            pos.z += direction.z;
        }
        break;
    }
    }
    float speed = *(float*)(*stack - (count - 3));
    MovePointInstanceView* data = ((ISceneNode*)object)->GetScriptObject()->user->GetScriptData();
    data->x = pos.x;
    data->y = pos.y;
    data->z = pos.z;
    filter->object->SetupArbitraryPointUpdate(type, (BS_STRUCT_Vector_struct*)&data->x, speed);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = found;
}
