// A fragment of objcreate.cpp (0x8003bf70): BSGO_Player's GetScriptData, GetSceneNode and GetProximityData: the script
// data at +16, the scene node at +44 and the proximity data at +48. They are
// weak copies of header inlines emitted in this file, so they are defined
// __declspec(weak); the members and result types are inferred. The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
struct ScriptDataView;
struct ProximityDataView;
class ISceneNode;

class BSGO_Player {
public:
    ScriptDataView* GetScriptData();
    ISceneNode* GetSceneNode();
    ProximityDataView* GetProximityData();

    unsigned char unknown00[16];
    unsigned char m_scriptData[28];
    ISceneNode* m_sceneNode;
    unsigned char m_proximityData[4];
};

__declspec(weak) ScriptDataView* BSGO_Player::GetScriptData() {
    return (ScriptDataView*)m_scriptData;
}

__declspec(weak) ISceneNode* BSGO_Player::GetSceneNode() {
    return m_sceneNode;
}

__declspec(weak) ProximityDataView* BSGO_Player::GetProximityData() {
    return (ProximityDataView*)m_proximityData;
}
