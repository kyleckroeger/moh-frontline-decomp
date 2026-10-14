// A fragment of anim_obj.cpp (0x80095b7c): CAnimObject::InitClass, the last
// function of the file, initialises the CAnimated and CCharacterShadow class
// data; its render-list and screen parameters are unused. The classes are
// views named by the mangled symbols.
class CRenderList;
class CScreen;

class CAnimated {
public:
    static void InitClass();
};

class CCharacterShadow {
public:
    static void InitClass();
};

class CAnimObject {
public:
    static void InitClass(CRenderList*, CScreen*);
};

void CAnimObject::InitClass(CRenderList*, CScreen*) {
    CAnimated::InitClass();
    CCharacterShadow::InitClass();
}
