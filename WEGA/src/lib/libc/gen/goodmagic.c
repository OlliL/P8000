/* @[$] goodmagic.c	2.1  09/12/83 11:24:52 - 87wega3.2 */

#include <s.out.h>

int swap_flg;
int magics[10] = {
	0x0000,
	N_MAGIC1,N_MAGIC3,N_MAGIC4,
	S_MAGIC1,S_MAGIC3,S_MAGIC4,
	X_MAGIC1,X_MAGIC3,X_MAGIC4
};

int
goodmagic(magic)
int magic;
{
	register int i;
	
	swap_flg=0x0000;
foo0:
	magics[0]=(unsigned)magic;

	i=10;
foo1:		
	i--;
	if(magic!=magics[i])
		goto foo1;
	if(i)
		goto foo2;
	magic=swap(magic);
	if(!swap_flg++)
		goto foo0;
foo2:
	if(i)
		goto foo3;
	swap_flg=0x0000;
foo3:
	return(i);
}
