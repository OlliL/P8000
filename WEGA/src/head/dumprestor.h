/* dumprestor.h	*/

#define NTREC   	8
#define MLEN    	16
#define MSIZ    	4096

#define TS_TAPE 	1
#define TS_INODE	2
#define TS_BITS 	3
#define TS_ADDR 	4
#define TS_END  	5
#define TS_CLRI 	6
#define MAGIC   	(int)60011
#define CHECKSUM	(int)84446
struct	spcl
{
	int	c_type;
	time_t	c_date;
	time_t	c_ddate;
	int	c_volume;
	daddr_t	c_tapea;
	ino_t	c_inumber;
	int	c_magic;
	int	c_checksum;
	struct	dinode	c_dinode;
	int	c_count;
	union {
		struct {
			char c_dev[30];
			char c_string[BSIZE-30];
		}	c_dinfo;
		char	c_addr[BSIZE];
	}c_block;
} spcl;

struct	idates
{
	char	id_name[16];
	char	id_incno;
	time_t	id_ddate;
};
