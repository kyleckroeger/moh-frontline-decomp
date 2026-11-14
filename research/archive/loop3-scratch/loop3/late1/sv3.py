from sv import *
B=open('sx66.cpp').read()
OLD="    ISceneNode* hi;\n    ISceneNode* lo;\n"
V={'lohi':B.replace(OLD,"    ISceneNode* lo;\n    ISceneNode* hi;\n"),
 'e_first':B.replace(OLD,"    CScene::SExclusionEntry* e;\n"+OLD).replace("    CScene::SExclusionEntry* e;\n    for (e =","    for (e ="),
 'e_first2':B.replace(OLD,"    CScene::SExclusionEntry* e;\n    ISceneNode* lo;\n    ISceneNode* hi;\n").replace("    CScene::SExclusionEntry* e;\n    for (e =","    for (e ="),
 'swapif':B.replace("""    if (a < b) {
        lo = a;
        hi = b;
    } else {
        lo = b;
        hi = a;
    }""","""    if (a < b) {
        hi = b;
        lo = a;
    } else {
        hi = a;
        lo = b;
    }"""),
}
for n,s in V.items(): print(n,score(s,n)[:170])
