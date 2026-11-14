OLD='''                int dx = (short)((int)(2670176.8f * (ax - 0.34906584f)) >> 8);
                int dy = (short)((int)(2670176.8f * az) >> 8);
                int dz = (short)((int)(2670176.8f * ay) >> 8);
                int x = angles[0];
                int y = angles[1];
                int z = angles[2];
                dx = (short)(dx - x);
                dy = (short)(dy - y);
                dz = (short)(dz - z);
                short sx = dx * t;
                short sy = dy * t;
                short sz = dz * t;
                angles[0] = sx + x;
                angles[1] = sy + y;
                angles[2] = sz + z;
'''
V=[]
def add(n,new): V.append((n,[(OLD,new)]))
add('I_pluseq','                short dx = (int)(2670176.8f * (ax - 0.34906584f)) >> 8;\n                short dy = (int)(2670176.8f * az) >> 8;\n                short dz = (int)(2670176.8f * ay) >> 8;\n                dx = dx - angles[0];\n                dy = dy - angles[1];\n                dz = dz - angles[2];\n                angles[0] += (short)(dx * t);\n                angles[1] += (short)(dy * t);\n                angles[2] += (short)(dz * t);\n')
add('J_pluseq2','                short dx = (short)((int)(2670176.8f * (ax - 0.34906584f)) >> 8) - angles[0];\n                short dy = (short)((int)(2670176.8f * az) >> 8) - angles[1];\n                short dz = (short)((int)(2670176.8f * ay) >> 8) - angles[2];\n                angles[0] += (short)(dx * t);\n                angles[1] += (short)(dy * t);\n                angles[2] += (short)(dz * t);\n')
add('K_tgt','                short tx = (int)(2670176.8f * (ax - 0.34906584f)) >> 8;\n                short ty = (int)(2670176.8f * az) >> 8;\n                short tz = (int)(2670176.8f * ay) >> 8;\n                short dx = tx - angles[0];\n                short dy = ty - angles[1];\n                short dz = tz - angles[2];\n                angles[0] += (short)(dx * t);\n                angles[1] += (short)(dy * t);\n                angles[2] += (short)(dz * t);\n')
add('L_tgt_noc','                short tx = (int)(2670176.8f * (ax - 0.34906584f)) >> 8;\n                short ty = (int)(2670176.8f * az) >> 8;\n                short tz = (int)(2670176.8f * ay) >> 8;\n                short dx = tx - angles[0];\n                short dy = ty - angles[1];\n                short dz = tz - angles[2];\n                angles[0] += dx * t;\n                angles[1] += dy * t;\n                angles[2] += dz * t;\n')
add('M_H_noint','                short dx = (short)((int)(2670176.8f * (ax - 0.34906584f)) >> 8);\n                short dy = (short)((int)(2670176.8f * az) >> 8);\n                short dz = (short)((int)(2670176.8f * ay) >> 8);\n                short x = angles[0];\n                short y = angles[1];\n                short z = angles[2];\n                dx = dx - x;\n                dy = dy - y;\n                dz = dz - z;\n                short sx = dx * t;\n                short sy = dy * t;\n                short sz = dz * t;\n                angles[0] = sx + x;\n                angles[1] = sy + y;\n                angles[2] = sz + z;\n')
add('N_H_plain','                short dx = (short)((int)(2670176.8f * (ax - 0.34906584f)) >> 8);\n                short dy = (short)((int)(2670176.8f * az) >> 8);\n                short dz = (short)((int)(2670176.8f * ay) >> 8);\n                int x = angles[0];\n                int y = angles[1];\n                int z = angles[2];\n                dx = dx - x;\n                dy = dy - y;\n                dz = dz - z;\n                angles[0] = (short)(dx * t) + x;\n                angles[1] = (short)(dy * t) + y;\n                angles[2] = (short)(dz * t) + z;\n')
add('O_H_sxint','                short dx = (short)((int)(2670176.8f * (ax - 0.34906584f)) >> 8);\n                short dy = (short)((int)(2670176.8f * az) >> 8);\n                short dz = (short)((int)(2670176.8f * ay) >> 8);\n                int x = angles[0];\n                int y = angles[1];\n                int z = angles[2];\n                dx = dx - x;\n                dy = dy - y;\n                dz = dz - z;\n                int sx = (short)(dx * t);\n                int sy = (short)(dy * t);\n                int sz = (short)(dz * t);\n                angles[0] = sx + x;\n                angles[1] = sy + y;\n                angles[2] = sz + z;\n')
add('P_H_inline','                short dx = (short)((int)(2670176.8f * (ax - 0.34906584f)) >> 8);\n                short dy = (short)((int)(2670176.8f * az) >> 8);\n                short dz = (short)((int)(2670176.8f * ay) >> 8);\n                int x = angles[0];\n                int y = angles[1];\n                int z = angles[2];\n                dx -= x;\n                dy -= y;\n                dz -= z;\n                short sx = dx * t;\n                short sy = dy * t;\n                short sz = dz * t;\n                angles[0] = sx + x;\n                angles[1] = sy + y;\n                angles[2] = sz + z;\n')
