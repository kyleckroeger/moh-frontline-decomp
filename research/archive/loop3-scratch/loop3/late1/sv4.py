from sv import *
B=open('sx29.cpp').read()
PH="""    for (ISceneNode** it = active.begin(); it != active.end(); it++) {
        ISceneNode* other;
"""
assert PH in B
V={
 'raw':B.replace(PH,"    ISceneNode** it;\n    ISceneNode* other;\n    for (it = active.m_data; it != active.m_data + active.m_size; it++) {\n"),
 'endc':B.replace(PH,"    ISceneNode** it;\n    ISceneNode* other;\n    ISceneNode** const end = active.end();\n    for (it = active.begin(); it != end; it++) {\n"),
 'firstc':B.replace(PH,"    ISceneNode** it;\n    ISceneNode* other;\n    ISceneNode** const first = active.begin();\n    for (it = first; it != active.end(); it++) {\n"),
 'idx':B.replace(PH,"    ISceneNode* other;\n    for (int k = 0; k < active.m_size; k++) {\n        ISceneNode** it = &active.m_data[k];\n"),
 'pre':B.replace(PH,"    ISceneNode** it;\n    ISceneNode* other;\n    for (it = active.begin(); it != active.end(); ++it) {\n"),
}
for n,s in V.items(): print(n,score(s,n)[:170])
