import sys,subprocess,os
d='/tmp/SCRATCH/'
w=os.environ['D41C']
subprocess.run(['python3',d+'refuse2.py',sys.argv[1]],check=True,env=dict(os.environ,D41C="(p3->labno>=10000&&p3->labno<20000&&lr>1&&tn>2)||(backloop(p1,p3)&&jn<=1&&pre==0)"))
subprocess.run(['python3',d+'winc.py',sys.argv[1]],check=True,env=dict(os.environ,D41C=w))
