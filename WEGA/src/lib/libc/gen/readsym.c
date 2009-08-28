/* @[$] readsym.c	2.2  09/12/83 11:29:50 - 87wega3.2 */

#include <stdio.h>
#include <s.out.h>

readsym(sym, stream)
struct {
    struct s_nlist bol;
    char   extra[128];
}      *sym;
FILE *stream;
{
	register long foo1;
	register int foo2;
	
	if(fread(sym,14,1,stream)!=1)
		return(0xffff);
	
	if((*sym->bol.sn_name)&128)
		return(0);
	
	foo1 = ((*sym->bol.sn_name)&127)-8;
	
	if(fread(sym,14,foo1,stream) == foo1)
		return(foo1);

	return();
}
