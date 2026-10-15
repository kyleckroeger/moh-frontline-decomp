// A fragment of projectilebullet.cpp (0x800d0ff8): CProjectileBullet::GetCollisionId returns the collision id at +36 (the member is
// inferred). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
enum EClsnId {};

class CProjectileBullet {
public:
    EClsnId GetCollisionId() const;

    unsigned char unknown00[36];
    EClsnId m_collisionId;
};

EClsnId CProjectileBullet::GetCollisionId() const {
    return m_collisionId;
}
