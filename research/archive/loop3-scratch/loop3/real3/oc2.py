exec(open('oc.py').read().split("V={'base'")[0])
import itertools
TOP='''            CMatrix tm;
            CVector3 local;'''
V={}
for b in ['sub','copyminus']:
  for decl in ['motion','away','away, motion','motion, away','position, away, motion','motion, away, position']:
    body=A.replace('            CVector3 away;\n','') if 'away' in decl else A
    mb=M(bodies[b]).replace('            CVector3 motion;\n','') if 'motion' in decl else M(bodies[b])
    new=OLD
    pairs=[MEM,(TOP,TOP+'\n            CVector3 '+decl+';')]
    rep=body+mb
    if 'position' in decl: pairs.append(('            CVector3 position;\n            player','            player'))
    pairs.append((OLD,rep))
    V[b+'_'+decl.replace(', ','')]=pairs
