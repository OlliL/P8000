# define	opGTGE		opGT
# define	opLTLE		opLT

struct simp
{
	int	relop;		/* value of relop in simp clause	*/
	int	att;		/* attno of attribute			*/
	SYMBOL	 *konst;	/* pointer to constant value symbol	*/
};

struct key
{
	SYMBOL	*keysym;
	int	dnumber;
};

extern struct simp	Simp[NSIMP];
extern int		Nsimp;	/* Current no. entries in Simp vector	*/
extern int		Fmode;	/* find-mode determined by strategy	*/
extern char		Keyl[MAXTUP];
extern char		Keyh[MAXTUP];

extern struct key	Lkey_struct[MAXDOM + 1];
extern struct key	Hkey_struct[MAXDOM + 1];
