S0='''    const CVector3& center = sphere.GetCenter();
    CVector3 diff;
    diff.Sub(center, m_center);
    CVector3 local(m_axes[0].Dot(diff), m_axes[1].Dot(diff), m_axes[2].Dot(diff));
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
DEP={1:[0],2:[1],3:[2],10:[9,3],11:[5],12:[6],13:[8]}
