/* Struktur des Object Modul Headers einer z.out file	*/
 
struct z_exec {
	int	z_magic;	/* Magic number */
	unsigned z_code;	/* size Code */
	unsigned z_data;	/* size Data */
	unsigned z_bss;		/* size BSS */
	unsigned z_syms;	/* size Symbol table */
	unsigned z_entry;	/* entry point */
	unsigned z_flag;
	unsigned z_unused;
	};
 
/* gueltige Magic Nummern sind E811, E810, E807 */
 
#define		Z_MAGIC1  0xE807	/* Nonsegmented executable */
#define		Z_MAGIC3  0xE811	/* Nonsegmented separate I & D */
