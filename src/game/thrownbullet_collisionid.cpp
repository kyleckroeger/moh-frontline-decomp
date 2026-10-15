// A fragment of thrownbullet.cpp (0x800d27d4): CThrownBullet::GetCollisionId returns the collision id at +36 (the member is
// inferred). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
enum EClsnId {};

class CThrownBullet {
public:
    EClsnId GetCollisionId() const;

    unsigned char unknown00[36];
    EClsnId m_collisionId;
};

EClsnId CThrownBullet::GetCollisionId() const {
    return m_collisionId;
}
