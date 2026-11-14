O1='''                    if (flat.Dot(front) < 0.0f) {
                        flat += (flat.Dot(front) * -2.0f) * front;
'''
V=[
('nx_k',[(O1,'''                    if (flat.Dot(front) < 0.0f) {
                        float nx = front.x;
                        float ny = front.y;
                        float nz = front.z;
                        float k = flat.Dot(front) * -2.0f;
                        flat.x += k * nx;
                        flat.y += k * ny;
                        flat.z += k * nz;
''')]),
('nx_pre',[(O1,'''                    float nx = front.x;
                    float ny = front.y;
                    float nz = front.z;
                    if (flat.Dot(front) < 0.0f) {
                        flat += (flat.Dot(front) * -2.0f) * CVector3(nx, ny, nz);
''')]),
('vec_n',[(O1,'''                    if (flat.Dot(front) < 0.0f) {
                        CVector3 n(front);
                        flat += (flat.Dot(n) * -2.0f) * n;
''')]),
('fwd',[(O1,'''                    const CVector3& n = front;
                    if (flat.Dot(n) < 0.0f) {
                        flat += (flat.Dot(n) * -2.0f) * n;
''')]),
]
