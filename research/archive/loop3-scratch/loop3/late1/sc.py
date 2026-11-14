import sys
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import os; F=[os.path.abspath(x) for x in sys.argv[1:]]; os.chdir('/home/kyle/workspace/moh-frontline')
import score
for f in F:
    print(f, score.real('config/GMFE69/staticobjfactory_helmet.json', open(f).read(), 'Init__20CStaticObjectFactoryFv','800b254c',3376))
