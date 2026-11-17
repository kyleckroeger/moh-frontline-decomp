// BSGO_Basic: the base of the script's game objects, shared by every file that uses it (PortReadiness
// step 3.3).
//
// From the original: __vt__10BSGO_Basic (weak): Destroy, GetScriptData, GetSceneNode,
// GetProximityData, GetEmbeddedHitReactionInfo, all inline (weak copies; Destroy does nothing, the
// others return 0). The table pointer is at +0xc, after 12 bytes of data, in every matched view.
//
// Inferred, not in any mangled name: the result types (pointers; 23 matched views say int for
// GetScriptData, 11 a pointer) and the data members, named by offset.
#ifndef GAME_BSGO_BASIC_H
#define GAME_BSGO_BASIC_H

class ISceneNode;

class BSGO_Basic {
public:
    int m_field0;
    int m_field4;
    int m_field8;

    virtual void Destroy() {}
    virtual void* GetScriptData() { return 0; }
    virtual ISceneNode* GetSceneNode() { return 0; }
    virtual void* GetProximityData() { return 0; }
    virtual void* GetEmbeddedHitReactionInfo() { return 0; }
};

#endif
