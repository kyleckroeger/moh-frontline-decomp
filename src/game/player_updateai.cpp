// A fragment of player.cpp (0x800a27d4): CPlayerObject::UpdateAI updates the player's AI doodad (+36). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class CAIPlayerDoodad {
public:
    void Update(float);
};

class CPlayerObject {
public:
    void UpdateAI(float);

    unsigned char unknown00[36];
    CAIPlayerDoodad m_doodad;
};

void CPlayerObject::UpdateAI(float delta) {
    m_doodad.Update(delta);
}
