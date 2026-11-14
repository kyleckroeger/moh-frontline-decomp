O1='''                    float d = flat.Dot(front);
                    if (d < 0.0f) {
                        float k = d * -2.0f;
                        flat += k * front;
'''
V=[
('twice',[(O1,'''                    if (flat.Dot(front) < 0.0f) {
                        flat += (flat.Dot(front) * -2.0f) * front;
''')]),
('twice_k',[(O1,'''                    if (flat.Dot(front) < 0.0f) {
                        float k = flat.Dot(front) * -2.0f;
                        flat += k * front;
''')]),
('twice_d',[(O1,'''                    if (flat.Dot(front) < 0.0f) {
                        float d = flat.Dot(front);
                        flat += (d * -2.0f) * front;
''')]),
('neg2',[(O1,'''                    float d = flat.Dot(front);
                    if (d < 0.0f) {
                        flat += (-2.0f * d) * front;
''')]),
('minus',[(O1,'''                    float d = flat.Dot(front);
                    if (d < 0.0f) {
                        float k = 2.0f * d;
                        flat -= k * front;
''')]),
]
