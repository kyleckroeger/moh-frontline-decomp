// A fragment of AISoldierDoodad.cpp (0x8005933c): CAISoldierDoodad::GetSceneNode returns the scene node at +80. It is a weak
// copy of a header inline emitted in this file, so it is defined
// __declspec(weak); the member is inferred. The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class ISceneNode;

class CAISoldierDoodad {
public:
    ISceneNode* GetSceneNode() const;

    unsigned char unknown00[80];
    ISceneNode* m_sceneNode;
};

__declspec(weak) ISceneNode* CAISoldierDoodad::GetSceneNode() const {
    return m_sceneNode;
}
