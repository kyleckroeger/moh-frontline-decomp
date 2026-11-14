O1='''                    if (flat.Dot(front) < 0.0f) {
                        flat += (flat.Dot(front) * -2.0f) * front;
                        scale = dir.x * dir.x + dir.y * dir.y;
'''
def f(a,b='                        scale = dir.x * dir.x + dir.y * dir.y;\n',c='                    if (flat.Dot(front) < 0.0f) {\n'):
    return [(O1,c+a+b)]
V=[
('a',f('                        flat += -2.0f * flat.Dot(front) * front;\n')),
('b',f('                        flat += front * (flat.Dot(front) * -2.0f);\n')),
('c',f('                        flat += (front.Dot(flat) * -2.0f) * front;\n')),
('d',f('                        flat += (flat.Dot(front) * -2.0f) * front;\n',c='                    if (front.Dot(flat) < 0.0f) {\n')),
('e',f('',b='                        scale = dir.x * dir.x + dir.y * dir.y;\n                        flat += (flat.Dot(front) * -2.0f) * front;\n')),
('g',f('                        flat -= (flat.Dot(front) * 2.0f) * front;\n')),
('h',f('                        flat += (2.0f * -flat.Dot(front)) * front;\n')),
('i',f('                        flat += (-flat.Dot(front) * 2.0f) * front;\n')),
('j',f('                        flat += (flat.Dot(front) * -2.0f) * front;\n',b='                        scale = dir.y * dir.y + dir.x * dir.x;\n')),
('k',f('                        flat += (flat.Dot(front) * -2.0f) * front;\n',b='                        scale = dir.x * dir.x;\n                        scale += dir.y * dir.y;\n')),
]
