from sv import *
B=open('sx50.cpp').read()
O1="""        ISceneNode* other;
        int order = GetCollisionOrder((*it)->GetCollisionId(), sceneNode->GetCollisionId());
        if (order) {
            other = *it;
"""
assert O1 in B
V={
'a':"""        int order;
        ISceneNode* other;
        order = GetCollisionOrder((*it)->GetCollisionId(), sceneNode->GetCollisionId());
        if (order) {
            other = *it;
""",
'b':"""        int order = GetCollisionOrder((*it)->GetCollisionId(), sceneNode->GetCollisionId());
        if (order) {
            ISceneNode* other = *it;
""",
'c':"""        ISceneNode* other;
        int order;
        order = GetCollisionOrder((*it)->GetCollisionId(), sceneNode->GetCollisionId());
        if (order) {
            other = *it;
""",
'd':"""        ISceneNode* other;
        int order = GetCollisionOrder((*it)->GetCollisionId(), sceneNode->GetCollisionId());
        if (order != 0) {
            other = *it;
""",
'e':"""        ISceneNode* other;
        signed char order = GetCollisionOrder((*it)->GetCollisionId(), sceneNode->GetCollisionId());
        if (order) {
            other = *it;
""",
}
for n,v in V.items(): print(n,score(B.replace(O1,v),'v8'+n)[:130])
