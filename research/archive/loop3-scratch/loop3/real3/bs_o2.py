S0='''    const CVector3& center = sphere.GetCenter();
    CVector3 diff;
    diff.Sub(center, m_center);
    CVector3 local;
    local.d.c.z = m_axes[2].Dot(diff);
    local.d.c.y = m_axes[1].Dot(diff);
    local.d.c.x = m_axes[0].Dot(diff);
    CVector3 normal;
    bool outside;
    float maxDist;
    CVector3 closest;
    float distSq;
    CVector3 abs;
    abs.Abs(local);
    outside = false;
    maxDist = -3.4028235e+38f;
    distSq = 0.0f;
'''
BLOCK=S0
S=[l.strip() for l in S0.strip('\n').split('\n')]
DEP={1:[0],2:[1],4:[2,3],5:[2,3],6:[2,3],13:[12,4,5,6],14:[9],15:[10],16:[11]}
