from sxv import *
decl='    dwi::fast_vec<ISceneNode*> active;\n'
top=FN[FN.index('{\n')+2:FN.index('    active.UseBuffer')]
lines=[l+'\n' for l in top.split('\n') if l.strip()]
sig=FN[:FN.index('{\n')+2]; body=FN[FN.index('    active.UseBuffer'):]
b1=body.replace('for (it = active.begin();','data = active.begin();\n            for (it = data;').replace('active.push_back(sceneNode);','data[active.m_size++] = sceneNode;')
b2=body.replace('for (it = active.begin();','data = active.begin();\n            for (it = data;')
b3=body.replace('active.push_back(sceneNode);','data = active.begin();\n            data[active.m_size++] = sceneNode;')
for bi,bb in enumerate([b1,b2,b3]):
  for p in range(len(lines)+1):
    L=lines[:p]+['    ISceneNode** data;\n']+lines[p:]
    print(bi,p,score(sub(sig+''.join(L)+bb)),flush=True)
