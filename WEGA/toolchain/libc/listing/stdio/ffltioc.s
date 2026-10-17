
fp		:= r15;
sp		:= r15;
/*~~_fltuse:*/

.code

__fltuse::
{
	ret
	~L1 := 0
	~L2 := 0
	}	/* __fltuse */

.data

/*~~_fegcvt:*/

.code

__fegcvt::
{
	ret
	~L1 := 0
	~L2 := 0
	}	/* __fegcvt */

.data

/*~~_inflt:*/

.code

__inflt::
{
	ret
	~L1 := 0
	~L2 := 0
	}	/* __inflt */

.data

