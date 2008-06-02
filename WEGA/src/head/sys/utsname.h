/* utsname.h */

#define UTS_LENGTH	9

struct utsname 
{
	char	sysname[UTS_LENGTH];
	char	nodename[UTS_LENGTH];
	char	release[UTS_LENGTH];
	char	version[UTS_LENGTH];
};

struct utsname 	utsname;
