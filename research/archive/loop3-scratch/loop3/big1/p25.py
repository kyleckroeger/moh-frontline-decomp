O1='''                    if (flat.Dot(front) < 0.0f) {
                        flat += (flat.Dot(front) * -2.0f) * front;
                        scale = dir.x * dir.x + dir.y * dir.y;
                    }
'''
V=[
('a',[(O1,'''                    if (flat.Dot(front) < 0.0f) {
                        scale = dir.x * dir.x + dir.y * dir.y;
                        flat += (flat.Dot(front) * -2.0f) * front;
                    }
''')]),
('b',[(O1,'''                    if (flat.Dot(front) < 0.0f) {
                        CVector3 n = (flat.Dot(front) * -2.0f) * front;
                        flat += n;
                        scale = dir.x * dir.x + dir.y * dir.y;
                    }
''')]),
('c',[(O1,'''                    if (flat.Dot(front) < 0.0f) {
                        flat.x += (flat.Dot(front) * -2.0f) * front.x;
                        flat.y += (flat.Dot(front) * -2.0f) * front.y;
                        flat.z += (flat.Dot(front) * -2.0f) * front.z;
                        scale = dir.x * dir.x + dir.y * dir.y;
                    }
''')]),
('d',[(O1,'''                    if (flat.Dot(front) < 0.0f) {
                        flat += (flat.Dot(front) * -2.0f) * front;
                        scale = dir.x * dir.x + dir.y * dir.y;
                    } else {
                    }
''')]),
('e',[(O1,'''                    if (flat.Dot(front) < 0.0f) {
                        flat += (flat.Dot(front) * -2.0f) * front;
                        float s = dir.x * dir.x + dir.y * dir.y;
                        scale = s;
                    }
''')]),
('f',[(O1,'''                    if (flat.Dot(front) < 0.0f) {
                        flat += (flat.Dot(front) * -2.0f) * front;
                        scale = dir.Dot2D();
                    }
'''),('    float Dot(const CVector3& v) const','    float Dot2D() const { return x * x + y * y; }\n    float Dot(const CVector3& v) const')]),
]
