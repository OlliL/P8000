import sys,subprocess
subprocess.run(['patch','-s','-p0',sys.argv[1]],stdin=open('/home/olivleh1/git/P8000/WEGA/host/recon/poptveto/z21_veto.patch'),check=True)
