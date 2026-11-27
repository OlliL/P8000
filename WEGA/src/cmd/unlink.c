static char sccsid[] = "@[$]unlink.c\t2.1  07/23/82 21:19:30 - 87wega3.2";
main(argc, argv) char *argv[]; {
	if(argc!=2) {
		write(2, "Usage: /etc/unlink name\n", 24);
		exit(1);
	}
	unlink(argv[1]);
	exit(0);
}
