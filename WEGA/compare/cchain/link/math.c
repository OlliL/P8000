long fac(n)
long n;
{
	return n <= 1 ? 1 : n * fac(n - 1);
}
int fib(n)
int n;
{
	return n < 2 ? n : fib(n - 1) + fib(n - 2);
}
main()
{
	printf("%ld %d %u\n", fac(10L), fib(15), 40000u / 7);
	return 0;
}
