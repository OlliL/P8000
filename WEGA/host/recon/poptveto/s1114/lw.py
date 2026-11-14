import sys,subprocess
for p in ['liv.py','win.py']:
    subprocess.run(['python3','/tmp/SCRATCH/vt/'+p,sys.argv[1]],check=True)
