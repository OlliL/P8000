
/*------------------------------------------------------------------*/
/*  M A K R O D E F I N I T I O N E N   fuer Generierung T Q L      */
/*------------------------------------------------------------------*/
/*			1= in Benutzung
			0= nicht in Benutzung 			    */
#ifdef psu
#define TOP		/* TQL/ESER nur fuer TOPAS */
#endif

#ifndef TOP

#define GEN_TOP  1
#define GEN_SFI  1
#define GEN_FIX  1
#define GEN_INT  0

#else

#define GEN_TOP  1	/* Generierung nur TOPAS */
#define GEN_SFI  0
#define GEN_FIX  0
#define GEN_INT  0

#endif

/*------------------------------------------------------------------*/
#if (GEN_TOP + GEN_SFI + GEN_FIX + GEN_INT) > 1
#define GENIF(X) \
	if(X)
#else
#define GENIF(X)
#endif
/*------------------------------------------------------------------*/
#if (GEN_TOP + GEN_SFI + GEN_FIX + GEN_INT) > 1
#define GENSWITCH(X) \
	switch(X)
#else
#define GENSWITCH(X)
#endif
/*------------------------------------------------------------------*/
#if (GEN_TOP + GEN_SFI + GEN_FIX + GEN_INT) > 1
#define GENCASE(X) \
	case (X):
#else
#define GENCASE(X)
#endif
/*------------------------------------------------------------------*/
#if (GEN_TOP + GEN_SFI + GEN_FIX + GEN_INT) > 1
#define GENBREAK \
	break;
#else
#define GENBREAK
#endif
/*------------------------------------------------------------------*/

