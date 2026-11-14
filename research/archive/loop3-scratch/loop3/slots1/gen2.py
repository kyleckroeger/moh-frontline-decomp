src=open('am0.cpp').read()
V={
'scope1':src.replace('left = view;','{ left = view; }'),
'scope2':src.replace('left = view;\n        left.x -= 1.0f;','{ left = view; left.x -= 1.0f; }'),
'ctorcast':src.replace('left = view;','left = CVector3(view);'),
'voidcast':src.replace('left = view;','(void)(left = view);'),
'forscope':src.replace('''        CVector3 left;
        left = view;''','''        CVector3 left;
        if (1) left = view;'''),
}
for k,v in V.items(): open('w_'+k+'.cpp','w').write(v)
