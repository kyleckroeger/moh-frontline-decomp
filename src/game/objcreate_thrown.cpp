// A fragment of objcreate.cpp (0x8003eb40): DeleteSpecialThrownObject (the
// thrown object cleans up, returns to the static-object factory, and its
// script object's game object goes back to the first object allocator before
// the script object is destroyed), then CreateSpecialThrownObject. A
// static object of the requested type is allocated from the factory (none:
// null); its thrown-object interface is initialised, given collision id 18
// and added to the scene. When the trigger names a script, a destructable
// script game object is allocated from the file's first object allocator
// (a class operator new, inferred) and built by the inline BSGO_Basic and
// BSGO_Destructable constructors; it is given the thrown object as its scene
// node, its proximity words are cleared and it takes the trigger's flag byte;
// its script object is created and handed to the thrown object, and the
// script receives event 44 with the given value. The
// thrown object is returned. The file name is this project's; the original
// record is objcreate.cpp. The classes, functions and globals are named by
// the symbols; the members, the trigger-record view, the event and collision
// values and the result type are inferred. CStaticObject's virtuals are
// declared in table order (placeholders for those not used here; CThrownObject's
// CleanUp at +356 after eight more), and the tables stay elsewhere.
class BSObject;
class ISceneNode;
class CThrownObject;

/* Inferred: the trigger's script record. */
struct TriggerInfoView {
    unsigned char unknown00[76];
    const char* scriptName;
    unsigned char unknown50[32];
    unsigned char flags;
};

struct TriggerObject_struct {
    int unknown00;
    TriggerInfoView* info;
};

class CStaticObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void SetCollisionId(int); /* EClsnId */
    virtual BSObject* GetScriptObject();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void Init();
    virtual void SetScript(BSObject*);
    virtual void unknown12c();
    virtual void unknown130();
    virtual void unknown134();
    virtual void unknown138();
    virtual void unknown13c();
    virtual CThrownObject* AsThrownObject();
};

class CThrownObject : public CStaticObject {
public:
    virtual void unknown144();
    virtual void unknown148();
    virtual void unknown14c();
    virtual void unknown150();
    virtual void unknown154();
    virtual void unknown158();
    virtual void unknown15c();
    virtual void unknown160();
    virtual void CleanUp();
};

class CStaticObjectFactory {
public:
    static CStaticObject* AllocateStaticObject(int, int);
    static void DeallocateStaticObject(CStaticObject*);
};

class CScene {
public:
    void Add(ISceneNode&);

    unsigned char unknown000[328];
};

extern CScene g_scene;

class BSUtilObjectInstanceMemoryAllocator {
public:
    void* GetFreeElement(bool);
    void FreeElement(void*);
};

extern BSUtilObjectInstanceMemoryAllocator g_pObjectAllocators[];

class BSGO_Basic {
public:
    BSObject* m_script;
    int m_field4;
    int m_field8;

    BSGO_Basic() : m_script(0), m_field4(0), m_field8(0) {}
    virtual void Destroy();
};

class BSGO_Destructable : public BSGO_Basic {
public:
    static void* operator new(unsigned long) { return g_pObjectAllocators[0].GetFreeElement(true); }
    BSGO_Destructable() {}
    virtual void Destroy();

    int m_flags;
    unsigned char unknown14[4];
    void* m_scriptData;
    ISceneNode* m_sceneNode;
    int m_proximity[4];
};

void CreateBSObject(const char*, BSObject**, BSGO_Basic*, TriggerObject_struct*, void**);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

/* Inferred: the script object's game object at +12. */
struct BSObjectView {
    unsigned char unknown00[12];
    void* gameObject;
};

void DestroyBSObject(BSObject*);

void DeleteSpecialThrownObject(CThrownObject* object) {
    object->CleanUp();
    CStaticObjectFactory::DeallocateStaticObject(object);
    BSObject* script = object->GetScriptObject();
    if (script) {
        g_pObjectAllocators[0].FreeElement(((BSObjectView*)script)->gameObject);
        DestroyBSObject(script);
    }
}

CThrownObject* CreateSpecialThrownObject(TriggerObject_struct* trigger, int type, int value) {
    CStaticObject* object;
    CThrownObject* thrown;
    TriggerInfoView* info;
    BSGO_Destructable* go;
    object = CStaticObjectFactory::AllocateStaticObject(type, -1);
    if (!object)
        return 0;
    thrown = object->AsThrownObject();
    thrown->Init();
    thrown->SetCollisionId(18);
    g_scene.Add(*(ISceneNode*)thrown);
    if (trigger) {
        info = trigger->info;
        if (info->scriptName) {
            go = new BSGO_Destructable;
            go->m_sceneNode = (ISceneNode*)thrown;
            go->m_proximity[1] = 0;
            go->m_proximity[0] = 0;
            go->m_proximity[3] = 0;
            go->m_proximity[2] = 0;
            go->m_flags = info->flags;
            CreateBSObject(info->scriptName, &go->m_script, go, trigger, &go->m_scriptData);
            thrown->SetScript(go->m_script);
            BSObjectTriggerEvent(go->m_script, 44, (void*)value, 0, false);
        }
    }
    return thrown;
}
