// A fragment of Soldier_object.cpp (0x800aabb8): CSoldierObject's
// GetBulletEmitterMatrix. With a weapon drawn (+17024) in the current slot
// (+17028; the slots at +16468), the matrix is the weapon's attachment bone
// (the slot's entry, 24 bytes, of the skeleton's table at +20 of +3300 names
// the bone; the bone matrices at +3904) times +9024, then the slot's matrix
// (+16512) and +17040 applied; otherwise it is the matrix at +16224. The
// classes and functions are named by the symbols; the CMatrix view follows
// Soldier_object_look.cpp and the members and the skeleton views are
// inferred from offsets (the array bounds only fill the gaps). The rest of the file is not part of this unit.
class CWeapon;

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);
    void Multiply(const CMatrix&, const CMatrix&);

    float m[4][4];
} __attribute__((aligned(16)));

/* inferred: a weapon slot's attachment entry (24 bytes), its bone at +16 */
struct AttachEntryView {
    unsigned char unknown00[16];
    int m_bone;
    unsigned char unknown14[4];
};

/* inferred: the skeleton's attachment table at +20 */
struct SkeletonView {
    unsigned char unknown00[20];
    AttachEntryView* m_attach;
};

class CSoldierObject {
public:
    void GetBulletEmitterMatrix(CMatrix&);

    unsigned char unknown0000[3300];
    SkeletonView* m_skeleton;
    unsigned char unknown0ce8[600];
    CMatrix m_bones[80];
    CMatrix m_matrix2340;
    unsigned char unknown2380[7136];
    CMatrix m_matrix3f60;
    unsigned char unknown3fa0[180];
    CWeapon* m_weapons[11];
    CMatrix m_slotMatrices[8];
    bool m_weaponDrawn;
    unsigned char unknown4281[3];
    int m_slot;
    unsigned char unknown4288[8];
    CMatrix m_matrix4290;
};

void CSoldierObject::GetBulletEmitterMatrix(CMatrix& matrix) {
    if (m_weaponDrawn && m_weapons[m_slot]) {
        matrix.Multiply(m_bones[m_skeleton->m_attach[m_slot].m_bone], m_matrix2340);
        matrix.Multiply(m_slotMatrices[m_slot], matrix);
        matrix.Multiply(m_matrix4290, matrix);
    } else {
        matrix = m_matrix3f60;
    }
}
