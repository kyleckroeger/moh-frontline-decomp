B='''    bool flag = **stack;
    bool result = StartWalk(object, flag);
'''
V={}
V['resfirst']=[(B,'''    bool result;
    bool flag = **stack;
    result = StartWalk(object, flag);
''')]
V['split']=[(B,'''    bool flag;
    bool result;
    flag = **stack;
    result = StartWalk(object, flag);
''')]
V['obj']=[(B,'''    CAIObject* ai = GetAIObject(object);
    bool flag = **stack;
    bool result = ai->StartCoverPointWalkToStored(flag);
''')]
V['obj2']=[(B,'''    bool flag = **stack;
    CAIObject* ai = GetAIObject(object);
    bool result = ai->StartCoverPointWalkToStored(flag);
''')]
V['direct']=[(B,'''    bool flag = **stack;
    bool result = GetAIObject(object)->StartCoverPointWalkToStored(flag);
''')]
V['argb']=[(B,'''    bool flag = BSArgB(stack);
    bool result = GetAIObject(object)->StartCoverPointWalkToStored(flag);
''')]
