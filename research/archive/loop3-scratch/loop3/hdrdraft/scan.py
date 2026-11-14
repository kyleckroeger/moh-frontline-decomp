import csv,re,sys
CL=["CVector3","CMatrix","ISceneNode","IMovingSceneNode","CPlane","CCamera","CPlayerObject","CScene","CScreen","CColor","CRect","CInputManager"]
MP={"DrawFragCount__13UserInterfaceFb","BIFunc_UpdateFragCount__FPPiPv","BIFunc_MPAddPlayerHealth__FPPiPv","InitMultiplayer__13CPlayerObjectFP12MPPlayerDatab"}
for r in csv.DictReader(open("research/drafts/index.tsv"),delimiter="\t"):
    if r["function"] in MP: continue
    t=open(r["draft"],errors="replace").read()
    has=[c for c in CL if re.search(r"^(?:class|struct)\s+"+c+r"\b[^;{]*\{",t,re.M)]
    dwi="dwi" if re.search(r"namespace\s+dwi",t) else ""
    inc=[c for c in CL if re.search(r'#include\s+"game/'+c+r'\.h"',t)]
    print(r["real_lines"] or "-", r["draft"].split("/")[-1][:50], ",".join(has), dwi, "inc:"+",".join(inc) if inc else "", sep="\t")
