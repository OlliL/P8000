extern int counter;
int counter = 3;
char *name = "two";
static int add(a, b)
int a, b;
{
	return a + b;
}
main()
{
	counter = add(counter, 4);
	printf("%s %d\n", name, counter);
	return 0;
}
