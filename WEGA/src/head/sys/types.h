/* types.h */

typedef	struct { int r[1]; } *	physadr;
typedef	unsigned short	ushort;
typedef	char		cnt_t;
typedef	long		daddr_t;
typedef char 		*caddr_t;
typedef	long		saddr_t;	/* segmented address */
typedef	long		paddr_t;	/* physical address */
typedef	long		uaddr_t;	/* user address */
typedef	unsigned int	ino_t;
typedef	long		time_t;
typedef	int		label_t[10];	/* regs 8-15, pcseg pcoff */
typedef	int		dev_t;
typedef	long		off_t;
