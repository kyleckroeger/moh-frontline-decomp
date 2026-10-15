// A fragment of player.cpp (0x8009d9d4): CPlayerObject::ExitLightVolume and
// EnterLightVolume forward the light volume to each of the 45 objects held
// at +1120 (null entries skipped) through CStaticObject's virtual
// EnterLightVolume and ExitLightVolume (+280 and +284). The classes are
// named by the mangled symbols; CPlayerObject is a non-virtual view whose
// object table is inferred from offsets, and CStaticObject's table is padded
// to those slots with placeholder entries (as in bsbifunc_attach.cpp). The
// rest of the file is not part of this unit.
struct BPDLightVolume;

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
    virtual void unknown05c();
    virtual void unknown060();
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
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
};

class CPlayerObject {
public:
    void ExitLightVolume(BPDLightVolume*);
    void EnterLightVolume(BPDLightVolume*);

    unsigned char unknown000[1120];
    CStaticObject* m_objects[45];
};

void CPlayerObject::ExitLightVolume(BPDLightVolume* volume) {
    for (int i = 0; i < 45; i++) {
        if (m_objects[i])
            m_objects[i]->ExitLightVolume(volume);
    }
}

void CPlayerObject::EnterLightVolume(BPDLightVolume* volume) {
    for (int i = 0; i < 45; i++) {
        if (m_objects[i])
            m_objects[i]->EnterLightVolume(volume);
    }
}
