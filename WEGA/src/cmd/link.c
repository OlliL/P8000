static char sccsid[] = "@[$]link.c\t2.1  07/23/82 20:26:16 - 87wega3.2";
main(argc, argv) char *argv[]; {
	if(argc!=3) {
		write(2, "Usage: /etc/link from to\n", 25);
		exit(1);
	}
	exit((link(argv[1], argv[2])==0)? 0: 2);
}
