"""Corpus of ar (host/recon/corpusrun.py): the scenario of every command (cases/scenario) and the table of contents of the
libraries of the system (cases/libc.a, slibc.a), each in five time zones (tv prints dates); the original against the shipped
build and the host build."""
NAME = 'ar'
ORIG = 'bin/ar'
PROG = 'ar'
CANDIDATES = {'wega': 'src/cmd/ar', 'host': 'host/ar/ar'}
ROOT_FILES = ['lib/libc.a', 'lib/slibc.a']
