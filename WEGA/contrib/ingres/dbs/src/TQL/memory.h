#define RM_MEMLEN sizeof(struct rm_memtab)
#define RM_HMEMTAB sizeof(struct rm_tabhead)

struct rm_memtab
{
	char mname[10];
	int moff;
	short mlen;
};

struct rm_tabhead
{
	char *zmemory;
	int lenmemory;
	int manz;
};
