// A fragment of hierobject.cpp (0x80098de4): weak copies of header inlines
// emitted in this file: CHierObject::Init (the static object's Init), its
// object type (8) and AsHierObject (itself). CHierObject and CStaticObject are
// named by the mangled symbols and are non-virtual views; the result types
// are inferred. GetBulletDamageMultiplierByClass follows them: for a part
// index above 0 the part object's, otherwise this object's two damage
// ranges (bullet-class minimum, maximum, multiplier; an inline loop, unrolled
// in the image) give the multiplier, 0 when none matches (a .sdata2 pool
// entry). RegisterBulletDamage stores a (minimum, maximum, multiplier) range
// in the first free slot (minimum -1) of the part's ranges (an inline) or, for
// part 0 and below, this object's own (the same loop written in place, which
// gives its registers). The part table at +1440 and the ranges at +1832 are
// inferred (the table's length only fills the gap). The rest of the file is
// not part of this unit.
class CStaticObject {
public:
    void Init();
};

class CHierObject : public CStaticObject {
public:
    void Init();
    int GetObjectType() const;
    const CHierObject* AsHierObject() const;
    CHierObject* AsHierObject();
    float GetBulletDamageMultiplierByClass(int, int);
    void RegisterBulletDamage(int, int, int, float);

    struct DamageRange {
        int minimum;
        int maximum;
        float multiplier;
    };

    float DamageMultiplier(int bulletClass) const {
        for (int i = 0; i < 2; i++) {
            if (bulletClass >= m_damageRanges[i].minimum && bulletClass <= m_damageRanges[i].maximum)
                return m_damageRanges[i].multiplier;
        }
        return 0.0f;
    }

    void AddDamageRange(int minimum, int maximum, float multiplier) {
        for (int i = 0; i < 2; i++) {
            if (m_damageRanges[i].minimum == -1) {
                m_damageRanges[i].minimum = minimum;
                m_damageRanges[i].maximum = maximum;
                m_damageRanges[i].multiplier = multiplier;
                return;
            }
        }
    }

    unsigned char unknown000[1440];
    CHierObject* m_parts[98];
    DamageRange m_damageRanges[2];
};

__declspec(weak) void CHierObject::Init() {
    CStaticObject::Init();
}

__declspec(weak) int CHierObject::GetObjectType() const {
    return 8;
}

__declspec(weak) const CHierObject* CHierObject::AsHierObject() const {
    return this;
}

__declspec(weak) CHierObject* CHierObject::AsHierObject() {
    return this;
}

float CHierObject::GetBulletDamageMultiplierByClass(int part, int bulletClass) {
    if (part > 0)
        return m_parts[part]->DamageMultiplier(bulletClass);
    return DamageMultiplier(bulletClass);
}

void CHierObject::RegisterBulletDamage(int part, int minimum, int maximum, float multiplier) {
    if (part > 0)
        m_parts[part]->AddDamageRange(minimum, maximum, multiplier);
    else
    for (int i = 0; i < 2; i++) {
        if (m_damageRanges[i].minimum == -1) {
            m_damageRanges[i].minimum = minimum;
            m_damageRanges[i].maximum = maximum;
            m_damageRanges[i].multiplier = multiplier;
            return;
        }
    }

}
