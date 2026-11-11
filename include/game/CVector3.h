// CVector3: the game's 3D vector, shared by every file that uses it (PortReadiness step 3.3).
//
// Layout: four floats, 16 bytes, 8-byte aligned (230 of the 256 matched views say so; the
// rest don't state an alignment). Copies move it as two doubles (lfd/stfd pairs), as the
// copy constructor and assignment of most matched views do.
//
// The fields are plain x, y, z, w. Matched views also spell them d.v[i], d.c.x or c.x
// (a union around them); the original can't have: MWCC 1.3.2 can't reach the members of
// an anonymous struct at all, inside the class or outside, even with gcc_extensions on.
// Moving a file to this header rewrites those spellings.
//
// Method bodies follow the original's weak copies where it has them, otherwise the most
// common form among the matched views. A fragment whose own
// view differs keeps it behind a guard until it is re-matched with this one; see
// research/tools/try_header.py and docs/PortReadiness.md.
#ifndef GAME_CVECTOR3_H
#define GAME_CVECTOR3_H

#include "game/msl_sqrtf.h"

struct CVector3XYZW {
    float x, y, z, w;
};

union CVector3Data {
    double pair[2];
    float v[4];
    CVector3XYZW c;
};

// Some views cast to this to copy a vector as two doubles.
struct CVector3Pair {
    double a;
    double b;
};

class CVector3 {
public:
    float x, y, z, w;

    // Bodies marked "original" are read from the weak copies the original image keeps
    // (inline functions the compiler didn't inline somewhere): Moh2RelGC.elf has
    // __as__, both constructors, Length, Normalize, Scale, Add, Sub, Abs, Cross and
    // DotDotDot of CVector3.
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}                // original
    // Copies move the 16 bytes as two doubles, like the original's weak __ct__ and __as__.
    // operator= returns a reference: with a by-value return the inlined assignment keeps
    // a dead copy on the stack that matched code doesn't have (e.g. CCamera::GetPosition).
    // The original's one out-of-line __as__ does return by value; the unit that emits it
    // keeps its own view (see research/layout/header_moves/CVector3.tsv).
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; } // original
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    void Set(float ax, float ay, float az) { x = ax; y = ay; z = az; }
    float LengthSq() const { return x * x + y * y + z * z; }
    float LengthSquared() const { return x * x + y * y + z * z; }
    float Length() const { return sqrtf(x * x + y * y + z * z); }               // original
    // original: returns the length; one divide, then three multiplies.
    float Normalize() {
        float length = Length();
        if (length != 0.0f) {
            float scale = 1.0f / length;
            x *= scale;
            y *= scale;
            z *= scale;
        }
        return length;
    }
    float Dot(const CVector3& o) const { return x * o.x + y * o.y + z * o.z; }
    void Cross(const CVector3& a, const CVector3& b) {                         // original
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }
    // original: each component of this is one vector's dot product with v.
    void DotDotDot(const CVector3& a, const CVector3& b, const CVector3& c, const CVector3& v) {
        x = a.x * v.x + a.y * v.y + a.z * v.z;
        y = b.x * v.x + b.y * v.y + b.z * v.z;
        z = c.x * v.x + c.y * v.y + c.z * v.z;
    }
    void Abs(const CVector3& o) { x = __fabs(o.x); y = __fabs(o.y); z = __fabs(o.z); } // original
    CVector3& operator+=(const CVector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    CVector3& operator-=(const CVector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    CVector3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
    void Scale(const CVector3& o, float k) { x = k * o.x; y = k * o.y; z = k * o.z; } // original
    void Sub(const CVector3& a, const CVector3& b) { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z; } // original
    void Add(const CVector3& o) { x += o.x; y += o.y; z += o.z; }              // original
    // Not in the original's weak copies; cdbgeom_triangle_line's form.
    void Unitize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
    void Constrain(const CVector3&, float); // a real function (Constrain__8CVector3FRC8CVector3f)
} __attribute__((aligned(8)));

#endif
