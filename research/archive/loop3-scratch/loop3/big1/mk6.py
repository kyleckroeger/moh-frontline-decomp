base_old='''                short* angles = m_boneAngles[bone];
                short dx = (short)((int)(2670176.8f * (ax - 0.34906584f)) >> 8);
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
F={}
F['noang']=base_old.replace('                short* angles = m_boneAngles[bone];\n','').replace('angles[','m_boneAngles[bone][')
F['angafter']=base_old.replace('                short* angles = m_boneAngles[bone];\n','').replace('                int x = angles[0];','                short* angles = m_boneAngles[bone];\n                int x = angles[0];')
F['angdecl']=base_old.replace('                short* angles = m_boneAngles[bone];\n','                short* angles;\n').replace('                int x = angles[0];','                angles = m_boneAngles[bone];\n                int x = angles[0];')
F['angarr']=base_old.replace('short* angles = m_boneAngles[bone];','short (&angles)[3] = m_boneAngles[bone];')
s="V=[]\n"
for k,v in F.items(): s+="V.append((%r,[(%r,%r)]))\n"%(k,base_old,v)
open('scratch/loop3/big1/p6.py','w').write(s)
