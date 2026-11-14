ADD=('''    void operator-=(const CVector3& other) {''','''    void Scale(const CVector3& o, float k) { x = k * o.x; y = k * o.y; z = k * o.z; }
    void Sub(const CVector3& a, const CVector3& b) { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z; }
    void operator-=(const CVector3& other) {''')
D1=('                    delta -= position;','                    delta.Sub(delta, position);')
D2=('''                    delta = owner->m_target;
                    position = owner->m_position;
                    delta -= position;''','''                    position = owner->m_position;
                    delta.Sub(owner->m_target, position);''')
SC=('''                        owner->m_unknown23d8.x = scale * move.x;
                        owner->m_unknown23d8.y = scale * move.y;
                        owner->m_unknown23d8.z = scale * move.z;''','                        owner->m_unknown23d8.Scale(move, scale);')
MV=('''                    move.x = move.x - owner->m_unknown23b8.x;
                    move.y = move.y - owner->m_unknown23b8.y;
                    move.z = move.z - owner->m_unknown23b8.z;''','                    move.Sub(move, owner->m_unknown23b8);')
V={'base':[],'d1':[ADD,D1],'d2':[ADD,D2],'sc':[ADD,SC],'mv':[ADD,MV],'d1scmv':[ADD,D1,SC,MV]}
