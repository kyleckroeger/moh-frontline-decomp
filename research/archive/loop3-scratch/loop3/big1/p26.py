O1='''                    if (flat.Dot(front) < 0.0f) {
                        flat += (flat.Dot(front) * -2.0f) * front;
'''
V=[
('dv_twice',[(O1,'''                    float d = flat.Dot(front);
                    if (d < 0.0f) {
                        flat += (flat.Dot(front) * -2.0f) * front;
''')]),
('assign_in_if',[(O1,'''                    float d;
                    if ((d = flat.Dot(front)) < 0.0f) {
                        flat += (d * -2.0f) * front;
''')]),
('d_inline_k',[(O1,'''                    float d = flat.Dot(front);
                    if (d < 0.0f) {
                        flat += (d * -2.0f) * front;
''')]),
('d_mul2',[(O1,'''                    float d = flat.Dot(front);
                    if (d < 0.0f) {
                        d *= -2.0f;
                        flat += d * front;
''')]),
('d_twice_k',[(O1,'''                    if (flat.Dot(front) < 0.0f) {
                        float d = flat.Dot(front);
                        d *= -2.0f;
                        flat += d * front;
''')]),
]
