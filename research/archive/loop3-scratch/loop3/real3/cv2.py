exec(open('cv.py').read().split('V={}')[0])
plain='''                float sx = b * bv.x;
                float tx = px * a;
                float ux = sx + tx;
                float dx = ux * 1.05f;
                float sy = b * bv.y;
                float ty = py * a;
                float uy = sy + ty;
                float dy = uy * 1.05f;
                float sz = b * bv.z;
                float tz = pz * a;
                float uz = sz + tz;
                float dz = uz * 1.05f;
                m_velocity.x = velocity.x - dx;
                m_velocity.y = velocity.y - dy;
                m_velocity.z = velocity.z - dz;'''
expr='''                m_velocity.x = velocity.x - (b * bv.x + px * a) * 1.05f;
                m_velocity.y = velocity.y - (b * bv.y + py * a) * 1.05f;
                m_velocity.z = velocity.z - (b * bv.z + pz * a) * 1.05f;'''
V={'base':[],'plain':[(SOL,plain)],'expr':[(SOL,expr)]}
