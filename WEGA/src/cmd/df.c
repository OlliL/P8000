static char sccsid[] = "@[$]df.c\t1.5  04/23/83 13:43:12 - 87wega3.2";
#include <stdio.h>
#include <sys/param.h>
#include <mnttab.h>
#include <sys/stat.h>
#include <sys/filsys.h>
#include <sys/fblk.h>
#include <nlist.h>
#define EQ(x,y,z) (strncmp(x,y,z)==0)
struct	mnttab	*M, *Mp;
struct stat	S;
struct filsys	sblock;
int	Flg, tflag;
int	fd;
int	nmount;
daddr_t	blkno	= 1;
daddr_t	alloc();
char	*calloc();

main(argc, argv)
char **argv;
{
	register fi, i;
	char	c;
	char	 dev[32];
	int	 j;

	nmount = getnmount(&M);
	while(argv[1][0] == '-') {
		switch(c = argv[1][1]) {
			case 'f':
				Flg++;
				break;

			case 'q':
				break;

			case 't':
				tflag=1;
				break;

			default:
				fprintf(stderr,"df: illegal arg -%c\n",c);
				exit(1);
		}
		argc--;
		argv++;
	}
	if((fi = open("/etc/mnttab", 0)) < 0) {
		fprintf(stderr,"df: cannot open /etc/mnttab\n");
		exit(1);
	}
	i = read(fi, M, nmount * sizeof(struct mnttab));
	close(fi);

	for(Mp = M; Mp < &M[nmount]; Mp++) {
		if(!Mp->mt_dev[0]) /* it's been umount'ed */
			continue;
		sprintf(dev, "/dev/%.8s", Mp->mt_dev);
		if(argc > 1) {
			for(j = 1; j < argc; ++j) {
				if(EQ(argv[j], dev, 14)
				|| EQ(argv[j], Mp->mt_dev, 8)
				|| EQ(argv[j], Mp->mt_filsys, 8)) {
					printit(dev, Mp->mt_filsys);
					argv[j][0] = '\0';
				}
			}
		} else
			printit(dev, Mp->mt_filsys);
	}
	for(i = 1; i < argc; ++i) {
		if(argv[i][0])
			printit(argv[i], "\0");
	}
	exit(0);
}

printit(dev, fs_name)
char *dev, *fs_name;
{
	daddr_t	i;

	if((fd = open(dev, 0)) < 0) {
		fprintf(stderr,"df: cannot open %s\n",dev);
		return;
	}
	if(!Flg) {
		if(stat(dev, &S) < 0) {
bad_dev:
			fprintf(stderr,"df: cannot stat %s\n",dev);
			return;
		}
		if((S.st_mode & S_IFMT) != S_IFBLK)
			goto bad_dev;
		sync();
		bread(1L, (char *) &sblock, sizeof(sblock));
		i = 0;
		while(alloc())
			i++;
		printf("%-8s(%-10s): %8ld blocks%8u i-nodes\n",fs_name,
			dev, i, sblock.s_tinode);
		if(tflag)
			printf("                     (%8ld total blocks,%5d for i-nodes)\n",
				sblock.s_fsize, sblock.s_isize);
	}
	else {
		sync();
		bread(1L, (char *) &sblock, sizeof(sblock));
		i = 0;
		while(alloc())
			i++;
		printf("%-8s(%-10s): %8ld blocks\n",fs_name, dev, i);
	}
	close(fd);
}

daddr_t
alloc()
{
	int i;
	daddr_t	b;
	struct fblk buf;

	i = --sblock.s_nfree;
	if(i<0 || i>=NICFREE) {
		printf("bad free count, b=%ld\n", blkno);
		return(0);
	}
	b = sblock.s_free[i];
	if(b == 0)
		return(0);
#ifdef SHIPPED
	if(b<sblock.s_isize || b>=sblock.s_fsize) {
#else
	if(b<sblock.s_isize || (long)b>=(long)sblock.s_fsize) {	/* the original compares signed: daddr_t was long then (a guess) */
#endif
		printf("bad free block (%ld)\n", b);
		return(0);
	}
	if(sblock.s_nfree <= 0) {
		bread(b, &buf, sizeof(buf));
		blkno = b;
		sblock.s_nfree = buf.df_nfree;
		for(i=0; i<NICFREE; i++)
			sblock.s_free[i] = buf.df_free[i];
	}
	return(b);
}

bread(bno, buf, cnt)
daddr_t bno;
char *buf;
{
	int n;
	extern errno;

	lseek(fd, bno<<BSHIFT, 0);
	if((n=read(fd, buf, cnt)) != cnt) {
		printf("read error %ld\n", bno);
		printf("count = %d; errno = %d\n", n, errno);
		exit(0);
	}
}

struct	nlist nl[] = {
	{ "_Nmount" },
	{ "" }
};

getnmount(mp)
struct mnttab **mp;
{
	int	kfd;
	int	n;

	nlist("/wega", nl);
	if(nl[0].nl_value == 0) {
		fprintf(stderr, "%s: No namelist\n", "/wega");
		exit(2);
	}
	if((kfd = open("/dev/kmem", 0)) == -1) {
		fprintf(stderr, "%s: Cannot open\n", "/dev/kmem");
		exit(2);
	}
	if(lseek(kfd, nl[0].nl_value, 0) == -1) {
		fprintf(stderr, "%s: Cannot reposition\n", "/dev/kmem");
		exit(2);
	}
	if(read(kfd, &n, 2) != 2) {
		fprintf(stderr, "%s: Cannot read\n", "/dev/kmem");
		exit(2);
	}
	*mp = (struct mnttab *)calloc(n, sizeof(struct mnttab));
	if(mp == 0) {	/* sic: the pointer, not the result of calloc */
		fprintf(stderr, "cannot allocate space for tables\n");
		exit(2);
	}
	return(n);
}
