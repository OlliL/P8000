#include <sys/types.h>

#define HEARD_IP		0x01
#define HEARD_ARP		0x02
#define HEARD_NETROM	0x04
#define HEARD_ROSE	0x08
struct heard_stuff {
	struct ax25 info;
	time_t htime;
	char *ifacename;
	int16 flags;
	short next;
};

#define MAX_HEARD 19  /* purely arbitrary, this limits to one screen */

struct ax25_heard {
	int16 enabled;
	int16 cnt;
	short first;
	struct heard_stuff list[MAX_HEARD];
};
