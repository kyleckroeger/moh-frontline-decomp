// Fragment of propdat.cpp: the property-record byte-order conversions
// (0x80040c6c-0x80042070) and the machine-gun lookups (to 0x800421e0). The rest of the original file is still original
// context. Structure follows Rising Sun's src/bpd/endian.cpp (CC0): every field
// is converted with ChangeEndian. Field types and names are views: offsets are
// established by the loads, and types by how each conversion is inlined (see
// docs/Game.md, "Endian conversions").

#include <math.h>

// Conversion helpers. Frontline inlines all of them. The scalar overloads swap
// bytes in a local copy; other 32-bit types go through the template, and floats
// through EndianSwap(float&, bool), whose out-of-line copy is a weak function in
// this file (outside this fragment). The bool argument is unused; its meaning
// is unknown.
inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

inline void ChangeEndian(unsigned int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<unsigned int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<unsigned int*>(bytes);
}

inline void ChangeEndian(unsigned short& value) {
    ChangeEndian(*reinterpret_cast<short*>(&value));
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

inline void EndianSwap(float& value, bool) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

inline void ChangeEndian(float& value) {
    float& f = value;
    EndianSwap(f, true);
}

// Only the converted fields and the position are declared. Field types at
// +0x40..+0x68 follow Rising Sun's view; the rest of the record is opaque here.
struct MOH_core_Struct {
    unsigned char unknown00[16];
    float position[3];
    unsigned char unknown1c[16];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char unknown34[2];
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[2];
    char* field40;
    unsigned long* field44;
    int field48;
    char* field4c;
    unsigned long field50;
    char* field54;
    unsigned long* field58;
    unsigned long* field5c;
    unsigned long* field60;
    unsigned long* field64;
    unsigned long* field68;
    unsigned char unknown6c[4];
};

struct MOH_fogParams_Struct {
    MOH_core_Struct core;
    short field70;
    short field72;
    short field74;
    unsigned char unknown76[2];
    int field78;
    int field7c;
};

struct MOH_animatedLight_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[2];
    short field72;
    unsigned long* field74;
    float* field78;
};

struct MOH_playerPathController_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[2];
    short field72;
    short field74;
    unsigned char unknown76[2];
    unsigned long field78;
    int field7c;
};

struct MOH_projectileGeneratorTarget_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[2];
    short field72;
    unsigned long field74;
    unsigned long field78;
    unsigned long field7c;
    unsigned long field80;
    unsigned long field84;
    int field88;
};

struct MOH_projectileGenerator_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[2];
    short field72;
    unsigned long field74;
    unsigned long field78;
    unsigned long field7c;
    unsigned long field80;
    unsigned long field84;
    unsigned long field88;
    unsigned long field8c;
    int field90;
    int field94;
};

struct MOH_particle_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[18];
    short field82;
    short field84;
    unsigned char unknown86[2];
    int field88;
    int field8c;
    int field90;
    int field94;
    int field98;
    int field9c;
    int fielda0;
    int fielda4;
    int fielda8;
    int fieldac;
    int fieldb0;
    int fieldb4;
    int fieldb8;
    int fieldbc;
    int fieldc0;
    int fieldc4;
    int fieldc8;
    int fieldcc;
    int fieldd0;
    int fieldd4;
    int fieldd8;
    int fielddc;
    int fielde0;
    int fielde4;
    int fielde8;
    void* fieldec;
};

struct MOH_mechanicEnvMod_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[4];
    int field74;
    int field78;
};

struct MOH_enemy_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[14];
    short field7e;
    short field80;
    short field82;
    short field84;
    short field86;
    short field88;
    short field8a;
    short field8c;
    short field8e;
    int field90;
    int field94;
    int field98;
    int field9c;
    int fielda0;
    int fielda4;
    int fielda8;
    int fieldac;
};

struct MOH_mechanic_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[4];
    int field74;
};

struct xyzProperty_Struct {
    int field00;
    unsigned long field04;
    int field08;
    int field0c;
    float field10;
    float field14;
    float field18;
    unsigned long field1c;
    float field20;
    float field24;
    float field28;
};

void EndianSwap(MOH_core_Struct& value);

void EndianSwap(MOH_fogParams_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field70);
    ChangeEndian(value.field72);
    ChangeEndian(value.field74);
    ChangeEndian(value.field78);
    ChangeEndian(value.field7c);
}

void EndianSwap(MOH_animatedLight_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field72);
    ChangeEndian(value.field74);
    ChangeEndian(value.field78);
}

void EndianSwap(MOH_playerPathController_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field72);
    ChangeEndian(value.field74);
    ChangeEndian(value.field78);
    ChangeEndian(value.field7c);
}

void EndianSwap(MOH_projectileGeneratorTarget_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field72);
    ChangeEndian(value.field74);
    ChangeEndian(value.field78);
    ChangeEndian(value.field7c);
    ChangeEndian(value.field80);
    ChangeEndian(value.field84);
    ChangeEndian(value.field88);
}

void EndianSwap(MOH_projectileGenerator_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field72);
    ChangeEndian(value.field74);
    ChangeEndian(value.field78);
    ChangeEndian(value.field7c);
    ChangeEndian(value.field80);
    ChangeEndian(value.field84);
    ChangeEndian(value.field88);
    ChangeEndian(value.field8c);
    ChangeEndian(value.field90);
    ChangeEndian(value.field94);
}

void EndianSwap(MOH_particle_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field82);
    ChangeEndian(value.field84);
    ChangeEndian(value.field88);
    ChangeEndian(value.field8c);
    ChangeEndian(value.field90);
    ChangeEndian(value.field94);
    ChangeEndian(value.field98);
    ChangeEndian(value.field9c);
    ChangeEndian(value.fielda0);
    ChangeEndian(value.fielda4);
    ChangeEndian(value.fielda8);
    ChangeEndian(value.fieldac);
    ChangeEndian(value.fieldb0);
    ChangeEndian(value.fieldb4);
    ChangeEndian(value.fieldb8);
    ChangeEndian(value.fieldbc);
    ChangeEndian(value.fieldc0);
    ChangeEndian(value.fieldc4);
    ChangeEndian(value.fieldc8);
    ChangeEndian(value.fieldcc);
    ChangeEndian(value.fieldd0);
    ChangeEndian(value.fieldd4);
    ChangeEndian(value.fieldd8);
    ChangeEndian(value.fielddc);
    ChangeEndian(value.fielde0);
    ChangeEndian(value.fielde4);
    ChangeEndian(value.fielde8);
    ChangeEndian(value.fieldec);
}

void EndianSwap(MOH_mechanicEnvMod_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field74);
    ChangeEndian(value.field78);
}

void EndianSwap(MOH_enemy_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field7e);
    ChangeEndian(value.field80);
    ChangeEndian(value.field82);
    ChangeEndian(value.field84);
    ChangeEndian(value.field86);
    ChangeEndian(value.field88);
    ChangeEndian(value.field8a);
    ChangeEndian(value.field8c);
    ChangeEndian(value.field8e);
    ChangeEndian(value.field90);
    ChangeEndian(value.field94);
    ChangeEndian(value.field98);
    ChangeEndian(value.field9c);
    ChangeEndian(value.fielda0);
    ChangeEndian(value.fielda4);
    ChangeEndian(value.fielda8);
    ChangeEndian(value.fieldac);
}

void EndianSwap(MOH_mechanic_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field74);
}

void EndianSwap(MOH_core_Struct& value) {
    ChangeEndian(value.field2c);
    ChangeEndian(value.field2e);
    ChangeEndian(value.field30);
    ChangeEndian(value.field32);
    ChangeEndian(value.field36);
    ChangeEndian(value.field38);
    ChangeEndian(value.field3a);
    ChangeEndian(value.field3c);
    ChangeEndian(value.field40);
    ChangeEndian(value.field44);
    ChangeEndian(value.field48);
    ChangeEndian(value.field4c);
    ChangeEndian(value.field50);
    ChangeEndian(value.field54);
    ChangeEndian(value.field58);
    ChangeEndian(value.field5c);
    ChangeEndian(value.field60);
    ChangeEndian(value.field64);
    ChangeEndian(value.field68);
}

void EndianSwap(xyzProperty_Struct& value) {
    ChangeEndian(value.field00);
    ChangeEndian(value.field04);
    ChangeEndian(value.field08);
    ChangeEndian(value.field0c);
    ChangeEndian(value.field10);
    ChangeEndian(value.field14);
    ChangeEndian(value.field18);
    ChangeEndian(value.field1c);
    ChangeEndian(value.field20);
    ChangeEndian(value.field24);
    ChangeEndian(value.field28);
}

// The vector, scene-node and trigger declarations below are views: only what
// these functions use is declared. CVector3 is 16 bytes and 8-byte aligned
// (its stack slots and doubleword copies); ISceneNode's virtual slots follow
// Frontline's ISceneNode table (GetPosition at +0x40).
#include "game/CVector3.h"


class CMatrix;
class CCollision;
class CDrawContext;
class CBullet;
class CLight;

#include "game/ISceneNode.h"



struct TriggerObject_struct {
    unsigned int flags;
    MOH_core_Struct* core;
};

// g_pMGTriggerObject holds 32 of these; only the trigger at +4 is read here.
struct MGTriggerObjectView {
    TriggerObject_struct* waypoint;
    TriggerObject_struct* mechanic;
    unsigned char unknown08[4];
};

extern MGTriggerObjectView g_pMGTriggerObject[32];

int SearchForClosestMachineGun(ISceneNode* node, float maxDistance) {
    float closest = 10000.0f;
    int found = -1;
    for (unsigned int i = 0; i < 32; i++) {
        TriggerObject_struct* mechanic = g_pMGTriggerObject[i].mechanic;
        if (mechanic) {
            CVector3 position;
            node->GetPosition(position);
            float dx = position.x - mechanic->core->position[0];
            float dy = position.y - mechanic->core->position[1];
            float dz = position.z - mechanic->core->position[2];
            float distance = sqrtf(dx * dx + dy * dy + dz * dz);
            if (distance < closest && distance < maxDistance) {
                closest = distance;
                found = i;
            }
        }
    }
    return found;
}

bool IsMGUsed(TriggerObject_struct* trigger) {
    return trigger->flags & 1;
}

void MarkMGAsUsed(TriggerObject_struct* trigger, bool used) {
    if (used)
        trigger->flags |= 1;
    else
        trigger->flags &= ~1;
}
