V={}
C='inline float Cross2(float ax, float ay, float bx, float by) { return ax * by - bx * ay; }'
V['c_a']=[(C,'inline float Cross2(float ax, float ay, float bx, float by) { return ax * by - ay * bx; }')]
V['c_b']=[(C,'inline float Cross2(float ax, float ay, float bx, float by) { return by * ax - bx * ay; }')]
V['c_c']=[(C,'inline float Cross2(float ax, float ay, float bx, float by) { return by * ax - ay * bx; }')]
V['kpost']=[('wa = k * Cross2(bx - x, by - y, cx - x, cy - y);','wa = Cross2(bx - x, by - y, cx - x, cy - y) * k;'),('wb = k * Cross2(cx - x, cy - y, -x, -y);','wb = Cross2(cx - x, cy - y, -x, -y) * k;'),('wc = k * Cross2(-x, -y, bx - x, by - y);','wc = Cross2(-x, -y, bx - x, by - y) * k;')]
V['wb_rev']=[('wb = k * Cross2(cx - x, cy - y, -x, -y);','wb = -k * Cross2(-x, -y, cx - x, cy - y);')]
V['locals']=[('    float k = -0.5f;\n','    float k = -0.5f;\n    float ax = bx - x;\n    float ay = by - y;\n    float bx2 = cx - x;\n    float by2 = cy - y;\n    float nx = -x;\n    float ny = -y;\n'),('wa = k * Cross2(bx - x, by - y, cx - x, cy - y);','wa = k * Cross2(ax, ay, bx2, by2);'),('wb = k * Cross2(cx - x, cy - y, -x, -y);','wb = k * Cross2(bx2, by2, nx, ny);'),('wc = k * Cross2(-x, -y, bx - x, by - y);','wc = k * Cross2(nx, ny, ax, ay);')]
