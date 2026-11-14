"""s.py d|c files... : real score of Draw (d) or ctor (c)"""
import sys
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import score
R='/home/kyle/workspace/moh-frontline/'
T={'d':(R+'config/GMFE69/scene.json','Draw__6CSceneFR9CViewportR7CScreenR11CRenderListR4CFog','800da67c',632),
   'c':(R+'config/GMFE69/scene_dtor.json','__ct__6CSceneFv','800dc8c4',900)}
t,f,a,s=T[sys.argv[1]]
for p in sys.argv[2:]: print(p,score.real(t,open(p).read(),f,a,s))
