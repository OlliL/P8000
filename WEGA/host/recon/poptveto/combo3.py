import sys,subprocess,os
d='/tmp/SCRATCH/'
subprocess.run(['python3',d+'fwd3.py',sys.argv[1]],check=True)
subprocess.run(['python3',d+'refuse.py',sys.argv[1]],check=True)
