O='                        flat += (flat.Dot(front) * -2.0f) * front;\n'
V=[
('fscale',[(O,'                        front *= flat.Dot(front) * -2.0f;\n                        flat += front;\n')]),
('fscale_k',[(O,'                        float k = flat.Dot(front) * -2.0f;\n                        front *= k;\n                        flat += front;\n')]),
('fscale_d',[(O,'                        front *= -2.0f * flat.Dot(front);\n                        flat += front;\n')]),
]
