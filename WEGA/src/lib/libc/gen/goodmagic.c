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
	register int ret;
	
	swap_flg=0x0000;

	do {
		magics[0]=magic;
		ret=sizeof(magics)/sizeof(int);

		do 
			ret--;
		while (magic!=magics[ret]);

		if(ret)
			break;

		magic=swap(magic);
	} while(!swap_flg++);

	if(!ret)
		swap_flg=0x0000;

	return(ret);
}
