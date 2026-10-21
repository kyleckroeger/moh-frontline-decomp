// A fragment of objcreate.cpp (0x8003c528): BSGO_Tank::Destroy and
// BSGO_Soldier::Destroy. Each unlinks the script game object from its list
// (the tank list, or the German list after counting one less active
// soldier), releases its scene object (a tank is removed from the scene and
// cleaned up through its table; a soldier is cleaned up and returned to the
// animated-object factory), destroys the script object, clears both pointers
// and returns the record to the second object allocator. Each Destroy is the
// key function of its class's table, so the unit holds __vt__9BSGO_Tank and
// __vt__12BSGO_Soldier with their type records (RTTI on); BSGO_Basic's
// file-local type record is a file duplicate of objcreate_volumes'. The file
// is compiled with deferred inlining (the tables come out in that order), so
// the functions are written in reverse image order. The classes, functions
// and globals are named by the symbols; BSGO_Basic's three words before its table pointer (the
// script object and the list links) follow objcreate_volumes.cpp, the
// scene-object members follow objcreate.cpp, and the tank's clean-up slot
// (+356), the Unlink inline and the accessors' result types are inferred.
class BSObject;
void DestroyBSObject(BSObject*);

class BSUtilObjectInstanceMemoryAllocator {
public:
    void FreeElement(void*);

    unsigned char unknown00[28];
};

class ISceneNode {
public:
    virtual void MarkForDestruction(int);
};

/* inferred: the tank's scene object, cleaned up through its slot +356 */
class TankNodeView : public ISceneNode {
public:
    virtual void vf00c();
    virtual void vf010();
    virtual void vf014();
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual void vf060();
    virtual void vf064();
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
    virtual void vf14c();
    virtual void vf150();
    virtual void vf154();
    virtual void vf158();
    virtual void vf15c();
    virtual void vf160();
    virtual void CleanUp();
};

class CScene {
public:
    void Remove(ISceneNode&);

    unsigned char unknown000[328];
};

extern CScene g_scene;

class CAnimObject;

class CSoldierObject {
public:
    void CleanUp();
};

class CAnimObjectFactory {
public:
    void ReleaseObject(CAnimObject*);

    unsigned char unknown00[28];
};

extern CAnimObjectFactory g_AnimObjFactory;

class BSGO_Basic {
public:
    BSObject* m_object;
    BSGO_Basic* m_next;
    BSGO_Basic* m_prev;

    virtual void Destroy();
    virtual void* GetScriptData(); /* result type not known */
    virtual ISceneNode* GetSceneNode();
    virtual void* GetProximityData(); /* result type not known */
    virtual void* GetEmbeddedHitReactionInfo(); /* result type not known */

    void Unlink(BSGO_Basic*& head) {
        if (m_prev)
            m_prev->m_next = m_next;
        if (m_next)
            m_next->m_prev = m_prev;
        if (head == this)
            head = m_next;
    }
};

class BSGO_Tank : public BSGO_Basic {
public:
    virtual void Destroy();
    virtual void* GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual void* GetProximityData();

    unsigned char m_script[212];
    TankNodeView* m_tank;
    unsigned char m_proximity[4];
};

class BSGO_Soldier : public BSGO_Basic {
public:
    virtual void Destroy();
    virtual void* GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual void* GetProximityData();
    virtual void* GetEmbeddedHitReactionInfo();

    unsigned char m_script[136];
    unsigned char m_hitReaction[76];
    CSoldierObject* m_soldier;
    unsigned char m_proximity[4];
};

extern BSGO_Basic* g_pTankList;
extern BSGO_Basic* g_pGermanList;
extern int g_numActiveSoldiers;
extern BSUtilObjectInstanceMemoryAllocator g_pObjectAllocators[2];

void BSGO_Soldier::Destroy() {
    g_numActiveSoldiers--;
    Unlink(g_pGermanList);
    m_soldier->CleanUp();
    g_AnimObjFactory.ReleaseObject((CAnimObject*)m_soldier);
    DestroyBSObject(m_object);
    m_object = 0;
    m_soldier = 0;
    g_pObjectAllocators[1].FreeElement(this);
}

void BSGO_Tank::Destroy() {
    Unlink(g_pTankList);
    g_scene.Remove(*m_tank);
    m_tank->CleanUp();
    DestroyBSObject(m_object);
    m_object = 0;
    m_tank = 0;
    g_pObjectAllocators[1].FreeElement(this);
}
