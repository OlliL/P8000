/*
 * UNIX stat.h
 */

#ifndef CLK_TCK
#include <time.h>
#endif

typedef char * dev_t;  /* pointer to the device name */

/*
 * file control structure
 */

struct stat
     {
     unsigned short  st_mode;
     unsigned short  st_nlink;
     unsigned short  st_uid;
     unsigned short  st_gid;
     long            st_size;
     time_t          st_atime;
     time_t          st_mtime;
     time_t          st_ctime;
     long            st_ino;
     dev_t           st_dev;
     dev_t           st_rdev;
     } ;

#ifndef S_IREAD
#include <modes.h>
#endif

