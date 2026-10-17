/* @[$] readsym.c	2.2  09/12/83 11:29:50 - 87wega3.2 */

#include <stdio.h>
#include <s.out.h>

readsym(sym, stream)
struct s_nlist *sym;
FILE *stream;
{
	register int foo2;
	char foo1;

	if(fread(sym,14,1,stream)!=1)
		return(0xffff);

	if(!((*sym->sn_name)&128))
		return(0);

	foo1 = ((*sym->sn_name)&127)-8;
	foo2 = (foo1+13)/14;
	sym++;

	if(fread(sym,14,foo2,stream) != foo2)
		foo2 = 0xffff;

	return(foo2);
}
