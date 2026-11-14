import sys,re
src=open('am0.cpp').read()
head,body=src.split('void CAnimMorph::Draw(CDrawContext& context) {')
variants={}
B=body
variants['assign']=(head,B.replace('left = view;','Assign(left, view);'))
variants['declTop']=(head,B.replace('        CVector3 left;\n','').replace('        CMatrix* world','        CVector3 left;\n        CMatrix* world',1))
variants['leftAfterRight']=(head,B.replace('        CVector3 left;\n        left = view;\n','').replace('        CVector3 right = view;\n','        CVector3 left;\n        CVector3 right = view;\n        left = view;\n'))
variants['posCtor']=(head,B.replace('''        CVector3 position;
        position.x = world->pos.x;
        position.y = world->pos.y;
        position.z = world->pos.z;''','        CVector3 position(world->pos.x, world->pos.y, world->pos.z);'))
for k,(h,b) in variants.items():
    open('v_'+k+'.cpp','w').write(h+'void CAnimMorph::Draw(CDrawContext& context) {'+b)
