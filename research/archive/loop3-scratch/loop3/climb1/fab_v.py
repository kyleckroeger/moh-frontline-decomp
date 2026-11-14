V={}
V['plain']=[('    blend[0] = blend[1] = blend[2] = blend[3] = blend[4] = 0.0f;','    blend[0] = 0.0f;\n    blend[1] = 0.0f;\n    blend[2] = 0.0f;\n    blend[3] = 0.0f;\n    blend[4] = 0.0f;')]
V['plainrev']=[('    blend[0] = blend[1] = blend[2] = blend[3] = blend[4] = 0.0f;','    blend[4] = 0.0f;\n    blend[3] = 0.0f;\n    blend[2] = 0.0f;\n    blend[1] = 0.0f;\n    blend[0] = 0.0f;')]
V['loop']=[('    blend[0] = blend[1] = blend[2] = blend[3] = blend[4] = 0.0f;','    for (int i = 0; i < 5; i++)\n        blend[i] = 0.0f;')]
