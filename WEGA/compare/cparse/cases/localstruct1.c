/* A structure declared inside a function: its tag is freed at the end of the function, the variables of the type keep pointing to it, and the
   next symbols reuse the block (cparse frees the symbols of an inner scope; the allocator gives the freed block to the next new symbol). */
int g1;
f(s)
char *s;
{
	struct tag {
		char *name;
		char *value;
	} *sp, *lookup();

	sp = lookup(s, 1);
	return sp->value == 0;
}
int g2, g3;
char *g4;
h()
{
	struct tag2 { int a; long b; } v, *p;

	p = &v;
	return p->a;
}
int g5(x) int x; { struct tag3 { char c[4]; } z; return z.c[x]; }
int g6;
