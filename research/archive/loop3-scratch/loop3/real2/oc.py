ADD=('''        } c;
    };
} __attribute__((aligned(8)));''','''        } c;
    };
    void Sub(const CVector3& a, const CVector3& b) { c.x = a.c.x - b.c.x; c.y = a.c.y - b.c.y; c.z = a.c.z - b.c.z; }
    float Dot(const CVector3& v) const { return c.x * v.c.x + c.y * v.c.y + c.z * v.c.z; }
    float Length() const { return sqrtf(c.x * c.x + c.y * c.y + c.z * c.z); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            float scale = 1.0f / length;
            c.x *= scale;
            c.y *= scale;
            c.z *= scale;
        }
    }
} __attribute__((aligned(8)));''')
AW=('''            away.c.x = point.c.x - position.c.x;
            away.c.y = point.c.y - position.c.y;
            away.c.z = point.c.z - position.c.z;
            Normalize(away);''','''            away.Sub(point, position);
            away.Normalize();''')
MO=('''            motion.c.x = point.c.x;
            motion.c.y = point.c.y;
            motion.c.z = point.c.z;
            motion.c.x -= local.c.x;
            motion.c.y -= local.c.y;
            motion.c.z -= local.c.z;
            Normalize(motion);''','''            motion.Sub(point, local);
            motion.Normalize();''')
DT=('away.c.x * motion.c.x + away.c.y * motion.c.y + away.c.z * motion.c.z > 0.0f','away.Dot(motion) > 0.0f')
V={'base':[],'aw':[ADD,AW],'mo':[ADD,MO],'dt':[ADD,DT],'all':[ADD,AW,MO,DT],'awdt':[ADD,AW,DT]}
AW2=(AW[0],'''            away.Sub(point, position);
            Normalize(away);''')
NM=('''    void Normalize() {
        float length = Length();''','''    void Normalize() {
        float length = sqrtf(c.x * c.x + c.y * c.y + c.z * c.z);''')
V={'aw2':[ADD,AW2,DT],'aw_nm':[ADD,NM,AW,DT],'n_only':[ADD,NM,('            Normalize(away);','            away.Normalize();'),('            Normalize(motion);','            motion.Normalize();'),DT],
'mo_copy':[ADD,AW2,DT,(MO[0],'''            motion = point;
            motion.c.x -= local.c.x;
            motion.c.y -= local.c.y;
            motion.c.z -= local.c.z;
            Normalize(motion);''')]}
V={'best':[ADD,NM,AW,DT,('            Normalize(motion);','            motion.Normalize();')]}
