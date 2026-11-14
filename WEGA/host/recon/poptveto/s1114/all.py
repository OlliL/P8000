import sys,subprocess
for p in sys.argv[2:] if len(sys.argv)>2 else ['vpatchX.py','liv.py']:
    subprocess.run(['python3','/tmp/SCRATCH/vt/'+p,sys.argv[1]],check=True)
