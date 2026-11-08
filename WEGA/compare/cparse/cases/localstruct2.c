/* Several inner structures and unions, the type used outside the function through an extern declaration, strings in between. */
char *msg1 = "first";
f1() { struct a { int x; } *p; extern struct a *q; return q != p; }
char *msg2 = "second string";
f2() { union u { int i; char c; } w; struct b { struct b *next; int v; } n, *m; m = &n; return w.i + m->v; }
int h1, h2, h3, h4;
f3(arg) struct { int i; } *arg; { struct c { char *s; } cc; static struct c sc; cc.s = "inner"; return arg->i + sc.s[0]; }
char *msg3 = "third";
int h5;
