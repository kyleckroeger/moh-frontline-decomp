// A fragment of sound.cpp (0x800eb824): PlayAnimTriggeredSound looks up an
// animation's sound event in the sound table and, when there is one, plays
// it as a scripted sound at the object's position. The classes, functions
// and g_soundMapSingleton are named by the symbols; the sound parameters
// (16 bytes) and the object's position slot (+64) are inferred. The rest of
// the file is in other units.
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

struct BSObject;

class CSoundTable {
public:
    struct SSoundParms {
        unsigned char unknown00[16];
    };

    bool GetSoundEvent(int, SSoundParms*) const;

    unsigned char unknown00[4];
};

extern CSoundTable g_soundMapSingleton;

class ISceneNode {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
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
    virtual void GetPosition(CVector3&) const;
};

class CAnimObject : public ISceneNode {
};

void PlayScriptedSound(ISceneNode*, CVector3, const CSoundTable::SSoundParms&, int, BSObject*);

void PlayAnimTriggeredSound(long id, CAnimObject* object) {
    CVector3 position;
    object->GetPosition(position);
    CSoundTable::SSoundParms parms;
    if (g_soundMapSingleton.GetSoundEvent(id, &parms))
        PlayScriptedSound(object, position, parms, -1, 0);
}
