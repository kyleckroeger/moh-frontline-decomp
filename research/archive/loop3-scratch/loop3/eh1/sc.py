import sys; sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); import score
T={'pum':('config/GMFE69/pumhandler.json','__ct__19PopUpMessageHandlerFPcPc','800ba3d0',988)}
t,f,a,s=T[sys.argv[1]]
for p in sys.argv[2:]: print(p, score.real('/home/kyle/workspace/moh-frontline/'+t,open(p).read(),f,a,s))
