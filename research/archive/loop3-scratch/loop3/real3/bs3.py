AB='    void Abs(const CVector3& o) { d.c.x = (float)fabs(o.d.c.x); d.c.y = (float)fabs(o.d.c.y); d.c.z = (float)fabs(o.d.c.z); }'
V={}
V['abs_zyx']=[(AB,'    void Abs(const CVector3& o) { d.c.z = (float)fabs(o.d.c.z); d.c.y = (float)fabs(o.d.c.y); d.c.x = (float)fabs(o.d.c.x); }')]
V['abs_inplace']=[(AB,AB+'\n    void Abs() { d.c.x = (float)fabs(d.c.x); d.c.y = (float)fabs(d.c.y); d.c.z = (float)fabs(d.c.z); }'),('    CVector3 abs;\n    abs.Abs(local);','    CVector3 abs = local;\n    abs.Abs();')]
V['abs_fabsf']=[(AB,'    void Abs(const CVector3& o) { d.c.x = fabsf(o.d.c.x); d.c.y = fabsf(o.d.c.y); d.c.z = fabsf(o.d.c.z); }')]
V['abs_ctor']=[('    CVector3 abs;\n    abs.Abs(local);','    CVector3 abs((float)fabs(local.d.c.x), (float)fabs(local.d.c.y), (float)fabs(local.d.c.z));')]
