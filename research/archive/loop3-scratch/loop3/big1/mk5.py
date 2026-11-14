base_old='''                short dx = (short)((int)(2670176.8f * (ax - 0.34906584f)) >> 8);
                short dy = (short)((int)(2670176.8f * az) >> 8);
                short dz = (short)((int)(2670176.8f * ay) >> 8);
                int x = angles[0];
                int y = angles[1];
                int z = angles[2];
                dx = dx - x;
                dy = dy - y;
                dz = dz - z;
                angles[0] = (short)(dx * t) + x;
                angles[1] = (short)(dy * t) + y;
                angles[2] = (short)(dz * t) + z;
'''
anchor='#define X_ZERO CVector3(0.0f, 0.0f, 0.0f)\n'
H={
'Ba':'static inline void BlendAngle(short* a, short target, float t) { short d = target - *a; *a += (short)(d * t); }\n',
'Bb':'static inline void BlendAngle(short* a, short target, float t) { int x = *a; short d = target - x; *a = (short)(d * t) + x; }\n',
'Bc':'static inline short BlendAngle(int x, short target, float t) { short d = target - x; return (short)(d * t) + x; }\n',
'Bd':'static inline void BlendAngle(short& a, short target, float t) { short d = target - a; a += (short)(d * t); }\n',
}
D='''                short dx = (short)((int)(2670176.8f * (ax - 0.34906584f)) >> 8);
                short dy = (short)((int)(2670176.8f * az) >> 8);
                short dz = (short)((int)(2670176.8f * ay) >> 8);
'''
calls={
'Ba':D+'''                BlendAngle(&angles[0], dx, t);
                BlendAngle(&angles[1], dy, t);
                BlendAngle(&angles[2], dz, t);
''',
'Bd':D+'''                BlendAngle(angles[0], dx, t);
                BlendAngle(angles[1], dy, t);
                BlendAngle(angles[2], dz, t);
''',
'Bc':D+'''                int x = angles[0];
                int y = angles[1];
                int z = angles[2];
                angles[0] = BlendAngle(x, dx, t);
                angles[1] = BlendAngle(y, dy, t);
                angles[2] = BlendAngle(z, dz, t);
''',
}
calls['Bb']=calls['Ba']
s="V=[]\n"
for k in H:
    s+="V.append((%r,[(%r,%r),(%r,%r)]))\n"%(k,anchor,H[k]+anchor,base_old,calls[k])
open('scratch/loop3/big1/p5.py','w').write(s)
