
fp		:= r15;
sp		:= r15;
/*~~_callex:*/

.code

__callex::
{
	callr	_exit
	ret
	~L1 := 0
	~L2 := 0
	}	/* __callex */

.data

