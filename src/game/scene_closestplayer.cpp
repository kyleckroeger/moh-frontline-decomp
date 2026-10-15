// A fragment of scene.cpp (0x800da540): CScene::FindClosestPlayer returns the
// player (the table at +8, its count at +76) whose position, through its
// GetPosition (virtual slot +64), is nearest the given point, or none. The
// class and function are named by the symbols; the scene and player views,
// the vector's double-pair view and its inline operations (the libc inline
// sqrtf for the length) are inferred. The rest of the file is in other units.
#include <math.h>

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { d = other.d; }
    void operator-=(const CVector3& o) {
        d.v[0] -= o.d.v[0];
        d.v[1] -= o.d.v[1];
        d.v[2] -= o.d.v[2];
    }
    float Length() const {
        float xx = d.v[0] * d.v[0];
        float yy = d.v[1] * d.v[1];
        float zz = d.v[2] * d.v[2];
        return sqrtf(xx + yy + zz);
    }

    CVector3Data d;
} __attribute__((aligned(8)));

class CPlayerObject {
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

class CScene {
public:
    CPlayerObject* FindClosestPlayer(CVector3);

    unsigned char unknown00[8];
    CPlayerObject* m_players[17];
    int m_playerCount;
};

CPlayerObject* CScene::FindClosestPlayer(CVector3 point) {
    float best = 3.4028235e+38f;
    CPlayerObject* closest = 0;
    for (int i = 0; i < m_playerCount; i++) {
        CVector3 position;
        m_players[i]->GetPosition(position);
        position -= point;
        float distance = position.Length();
        if (distance < best) {
            best = distance;
            closest = m_players[i];
        }
    }
    return closest;
}
