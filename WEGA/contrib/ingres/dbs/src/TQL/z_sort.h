#	include	<descrip.h>

#define	D_Z	DSC$K_DTYPE_Z		/* unspecified */
#define D_B	DSC$K_DTYPE_B		/* byte integer;  8-bit signed 2's-complement integer */
#define D_BU	DSC$K_DTYPE_BU		/* byte logical;  8-bit unsigned quantity */
#define D_W	DSC$K_DTYPE_W		/* word integer;  16-bit signed 2's-complement integer */
#define D_WU	DSC$K_DTYPE_WU		/* word logical;  16-bit unsigned quantity */
#define D_L	DSC$K_DTYPE_L		/* longword integer;  32-bit signed 2's-complement integer */
#define D_LU	DSC$K_DTYPE_LU		/* longword logical;  32-bit unsigned quantity */
#define D_Q	DSC$K_DTYPE_Q		/* quadword integer;  64-bit signed 2's-complement integer */
#define D_QU	DSC$K_DTYPE_QU		/* quadword logical;  64-bit unsigned quantity */
#define D_O	DSC$K_DTYPE_O		/* octaword integer;  128-bit signed 2's-complement integer */
#define D_OU	DSC$K_DTYPE_OU		/* octaword logical;  128-bit unsigned quantity */
#define D_F	DSC$K_DTYPE_F		/* F_floating;  32-bit single-precision floating point */
#define D_D	DSC$K_DTYPE_D		/* D_floating;  64-bit double-precision floating point */
#define D_G	DSC$K_DTYPE_G		/* G_floating;  64-bit double-precision floating point */
#define D_H	DSC$K_DTYPE_H		/* H_floating;  128-bit quadruple-precision floating point */
#define D_T	DSC$K_DTYPE_T		/* character-coded text;  a single character or a string */
#define D_NU	DSC$K_DTYPE_NU		/* numeric string, unsigned */
#define D_NL	DSC$K_DTYPE_NL		/* numeric string, left separate sign */
#define D_NLO	DSC$K_DTYPE_NLO		/* numeric string, left overpunched sign */
#define D_NR	DSC$K_DTYPE_NR		/* numeric string, right separate sign */
#define D_NRO	DSC$K_DTYPE_NRO		/* numeric string, right overpunched sign */
#define D_NZ	DSC$K_DTYPE_NZ		/* numeric string, zoned sign */
#define D_P	DSC$K_DTYPE_P		/* packed decimal string */

#define	RECORD	0			/* RECORD-Sortierprozess */
#define	TAG	1			/* TAG-Sortierprozess */

struct	sort_field			/* Sortierfeldaufbau System-Sort */
{
	short	s_typ;			/* Sortiertyp */
	short	s_order;		/* Sortierrichtung */
	short	s_offset;		/* Offset im Sort-Feld */
	short	s_len;			/* Laenge Sort-Kompon. */
};
