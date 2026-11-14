O1='''                    float d = flat.Dot(front);
                    if (d < 0.0f) {
                        float k = d * -2.0f;
                        flat += k * front;
                        scale = dir.x * dir.x + dir.y * dir.y;
                    }
'''
def v(dline,body):
    return O1.replace('                    float d = flat.Dot(front);\n',dline).replace('''                        float k = d * -2.0f;
                        flat += k * front;
                        scale = dir.x * dir.x + dir.y * dir.y;
''',body)
B0='''                        float k = d * -2.0f;
                        flat += k * front;
                        scale = dir.x * dir.x + dir.y * dir.y;
'''
D0='                    float d = flat.Dot(front);\n'
V=[
('d_expl',[(O1,v('                    float d = flat.x * front.x + flat.y * front.y + flat.z * front.z;\n',B0))]),
('d_yxz',[(O1,v('                    float d = flat.y * front.y + flat.x * front.x + flat.z * front.z;\n',B0))]),
('scale_first',[(O1,v(D0,'''                        float k = d * -2.0f;
                        scale = dir.x * dir.x + dir.y * dir.y;
                        flat += k * front;
'''))]),
('scale_first2',[(O1,v(D0,'''                        scale = dir.x * dir.x + dir.y * dir.y;
                        float k = d * -2.0f;
                        flat += k * front;
'''))]),
('noK',[(O1,v(D0,'''                        flat += d * -2.0f * front;
                        scale = dir.x * dir.x + dir.y * dir.y;
'''))]),
('fieldwise',[(O1,v(D0,'''                        float k = d * -2.0f;
                        flat.x += k * front.x;
                        flat.y += k * front.y;
                        flat.z += k * front.z;
                        scale = dir.x * dir.x + dir.y * dir.y;
'''))]),
('kdecl',[(O1,v(D0,'''                        float k;
                        k = d * -2.0f;
                        flat += k * front;
                        scale = dir.x * dir.x + dir.y * dir.y;
'''))]),
('tmpvec',[(O1,v(D0,'''                        CVector3 push = (d * -2.0f) * front;
                        flat += push;
                        scale = dir.x * dir.x + dir.y * dir.y;
'''))]),
]
