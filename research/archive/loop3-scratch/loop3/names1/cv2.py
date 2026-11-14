exec(open('/home/kyle/workspace/moh-frontline/scratch/loop3/names1/cv.py').read())
V={}
PN1='''            p = n2;
            p *= d2;
            e = p.Dot(n1);'''
V['p_copy_mul']=[HDR, DECL, (B1_OLD,B1_NEW), PDECL, PDECL2, (P_OLD,PN1), (OLDS, T2)]
V['p_copy_mul_T1']=[HDR, DECL, (B1_OLD,B1_NEW), PDECL, PDECL2, (P_OLD,PN1), (OLDS, T1)]
PN2='''            p.Scale(n2, d2);
            e = n1.x * p.x + n1.y * p.y + n1.z * p.z;'''
V['p_scale_e_plain']=[HDR, DECL, (B1_OLD,B1_NEW), PDECL, PDECL2, (P_OLD,PN2), (OLDS, T2)]
# keep scalars; tail as vector built from scalars via ctor-less: s.Scale(bv,b); then p via Set
V['scalar_p_tail_mix']=[HDR, DECL, (B1_OLD,B1_NEW), (OLDS,'''                CVector3 s;
                s.Scale(bv, b);
                float tx = px; tx *= a;
                float ty = py; ty *= a;
                float tz = pz; tz *= a;
                s.x += tx; s.y += ty; s.z += tz;
                s *= 1.05f;
                m_velocity.Sub(velocity, s);''')]
