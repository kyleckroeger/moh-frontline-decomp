import sys
sys.path.insert(0,'.'); import sc
S=sc.RS()
s=open('RS_BEST514.cpp').read()
def fr(src):
    m=S.asm(src)
    if m is None: return None
    return [x for x in m[:4] if 'subfic' in x or 'stwu' in x][:1]
V={}
V['base']=s
V['force_tmp']=s.replace('        force.Scale(drift, seconds2);\n','        CVector3 ft;\n        ft.Scale(drift, seconds2);\n        force = ft;\n')
V['off_tmp']=s.replace('        offset.Scale(pull, emitTime);\n','        CVector3 ot;\n        ot.Scale(pull, emitTime);\n        offset = ot;\n')
V['acc_out']=s.replace('        CVector3 acc;\n','').replace('    CVector3 offset, force,','    CVector3 acc;\n    CVector3 offset, force,')
V['axis_top']=s.replace('    CVector3 axis;\n','').replace('    CVector3 right, up;\n','    CVector3 right, up, axis;\n')
for k,v in V.items(): print(k,S(v),fr(v),flush=True)
