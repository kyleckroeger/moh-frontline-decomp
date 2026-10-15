// A fragment of decal.cpp (0x80083c44): CBulletDecalManager::InitClass keeps the render list and the screen (both
// file-local in the original and declared extern here). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class CRenderList;
class CScreen;

class CBulletDecalManager {
public:
    static void InitClass(CRenderList*, CScreen*);
};

extern CRenderList* g_pRenderList; // file-local in the original
extern CScreen* g_pScreen;         // file-local in the original

void CBulletDecalManager::InitClass(CRenderList* list, CScreen* screen) {
    g_pRenderList = list;
    g_pScreen = screen;
}
