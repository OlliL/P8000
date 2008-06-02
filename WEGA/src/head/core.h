/* core.h */

/* machine dependent stuff for core files */
#define TXTRNDSIZ 256L
#define stacktop(siz) (0x10000L)
#define stackbas(siz) (0x10000L-siz)
