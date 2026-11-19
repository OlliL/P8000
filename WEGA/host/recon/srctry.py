#!/usr/bin/env python3
"""srctry.py - try source variants of one function against the original (fnc.py), at most a few per call.

Usage: RECON_PROG=NAME srctry.py FILE.c FUNC < variants
variants (stdin): blocks separated by a line `=====`; each block `OLD` + a line `-----` + `NEW` (OLD is text of FILE.c, it occurs once).
Each variant is tried alone against the original text; prints fnc.py's line per variant, stops at the first OK (the file keeps that
variant), else the file is restored.
"""
import sys,subprocess,re,shutil,os
# usage: try.py FILE FUNC  (reads variants from stdin: blocks separated by a line '=====': OLD\n-----\nNEW)
f,fn=sys.argv[1],sys.argv[2]
src=open(f).read()
bak=src
vs=sys.stdin.read().split('\n=====\n')
for v in vs:
    old,new=v.split('\n-----\n')
    old=old.strip('\n'); new=new.strip('\n')
    if old not in bak: print('NO MATCH',old[:40]); continue
    open(f,'w').write(bak.replace(old,new,1))
    r=subprocess.run(['python3','host/recon/fnc.py',f,fn],capture_output=True,text=True)
    print(r.stdout.strip(), '|', new.replace('\n',' ')[:90])
    if r.stdout.split()[1]=='OK':
        print('FOUND'); sys.exit(0)
open(f,'w').write(bak)
