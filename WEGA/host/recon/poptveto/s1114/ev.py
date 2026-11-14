import sys,os
os.environ['FVETO_DIR']='/tmp/SCRATCH/fv'
sys.path.insert(0,'host/recon/poptveto')
import fveto
fveto.setup(sys.argv[1])
V=[int(x) for x in sys.argv[3:]]
v,n,ev=fveto.run(sys.argv[2],'jpr',0,V)
print('lines',n)
for e in ev: print(e, end=' ')
print()
