int gcom1, gcom2[10];
static int sbss[7];
int ginit = 5;
char gstr[] = "hello";
extern int b1(), bc;
int (*fp)() = b1;
int *ptr = &gcom1;
main(argc, argv) char **argv; { 
  sbss[3] = bc; gcom2[1] = b1(argc); printf("%d %s %x %x %x\n", ginit, gstr, 1, 2, 3); return fp(1); }
