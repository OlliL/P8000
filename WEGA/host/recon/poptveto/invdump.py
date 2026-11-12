import sys
p=sys.argv[1]; s=open(p).read()
a='''	loopiv++;
	nchange++;
	return p3;'''
assert s.count(a)==1
s=s.replace(a,'''	loopiv++;
	nchange++;
#ifdef TRACE
	if (debug) {
		printf("/*AFTERINV*/\\n");
		dumpnodes();
	}
#endif
	return p3;''')
a2='''				decref(p1->ref);
				return p2;
			}
		}
		return p1;'''
if s.count(a2)==1:
    s=s.replace(a2,'''#ifdef TRACE
				if (debug) {
					printf("/*AFTERCM*/\\n");
					dumpnodes();
				}
#endif
				decref(p1->ref);
				return p2;
			}
		}
		return p1;''')
open(p,'w').write(s)
