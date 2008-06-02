/* mnttab.h */

/* Format of the /etc/mnttab file which is set by the mount(M)
 * command
 */
#define NAMSIZ 32

struct mnttab {
	char	mt_dev[NAMSIZ],
		mt_filsys[NAMSIZ];
		short	mt_ro_flg;
	time_t	mt_time;
};
