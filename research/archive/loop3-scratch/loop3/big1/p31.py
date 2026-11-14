A='''                    CMatrix body;
                    float scale = -1.0f;
                    body.Multiply(m_bones[4], m_matrix);
'''
E='                    float e = front.Dot(side);\n'
V=[
('e_top',[(A,'''                    CMatrix body;
                    float scale = -1.0f;
                    float e;
                    body.Multiply(m_bones[4], m_matrix);
'''),(E,'                    e = front.Dot(side);\n')]),
('e_top0',[(A,'''                    CMatrix body;
                    float e;
                    float scale = -1.0f;
                    body.Multiply(m_bones[4], m_matrix);
'''),(E,'                    e = front.Dot(side);\n')]),
('scale_late',[(A,'''                    CMatrix body;
                    float scale;
                    body.Multiply(m_bones[4], m_matrix);
'''),('                    flat.Normalize();\n','                    flat.Normalize();\n                    scale = -1.0f;\n')]),
('scale_after_body',[(A,'''                    CMatrix body;
                    body.Multiply(m_bones[4], m_matrix);
'''),('                    CVector3 front;\n','                    CVector3 front;\n                    float scale = -1.0f;\n')]),
('e_noloc',[(E,''),('                    if (e > 0.8660254f) {','                    if (front.Dot(side) > 0.8660254f) {'),('                    } else if (e < -0.8660254f) {','                    } else if (front.Dot(side) < -0.8660254f) {')]),
]
