/**************************************************************************
 **************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1985
	Standalone:	sa.format.c
 
 
	Bearbeiter	: P. Hoge
	Datum		: 04/01/87 11:15:41
	Version		: 1.1
 
 **************************************************************************
 **************************************************************************/
 
#define NSECTS 18

char buf[10];
unsigned int drive;
unsigned int nsect;
unsigned int fhead;
unsigned int lhead;
unsigned int fcyl;
unsigned int lcyl;
 
main()
{
	register cyl;
	register head;
	register error;
	register sect;

	printf("\nFormat Winchester-Drive\n");
	do {
		printf("Drive: ");
		drive = input();
	}while(drive > 1);
 
	do {
		printf("First Head: ");
		fhead = input();
	}while(fhead > 8);

	do {
		printf("Last Head: ");
	lhead = input();
	}while(lhead > 8 || fhead > lhead);
 
	nsect = NSECTS;

	do {
		printf("First Cylinder: ");
		fcyl = input();
	}while(fcyl > 1023);

	do {
		printf("Last Cylinder: ");
		lcyl = input();
	}while(lcyl > 1023 || fcyl > lcyl);
 
	printf("Format: drive %d, %d sectors/track, cylinder %d - %d, head %d - %d\n", drive, nsect, fcyl, lcyl, fhead, lhead);
	printf("Ready to Format ?: ");
	inyes();
	for (cyl=fcyl; cyl<=lcyl; cyl++) {
	 for (head=fhead; head <= lhead; head++) {
	  printf("Format - cylinder: %d  head: %d\r", cyl, head);
	  error = fdisk(3, drive, head, nsect, 1, cyl);
	  if ( error != 0) {
	   printf("\nError 0x%x - continue ?: ", error);
	   inyes();
	  }
	 }
	}

	for (cyl=fcyl; cyl<=lcyl; cyl++) {
	 for (head=fhead; head <= lhead; head++) {
	  printf("Verify - cylinder: %d  head: %d   \r", cyl, head);
	  for(sect = 1; sect <= nsect; sect++) {
	   error = fdisk(1, drive, head, nsect, sect, cyl);
	   if (error != 0) {
	    printf("\nError 0x%x on sector %d - continue ?: ", error, sect);
	    inyes();
	   }
	  }
	 }
	}
	printf("\n");
	exit();
}

input()
{
	gets(buf, 10);
	if (buf[0] < '0' || buf[0] > '9')
		exit();
	return(atoi(buf));
}

inyes()
{
	gets(buf, 3);
	if (buf[0] == 'y' || buf[0] == 'Y')
		return;
	exit();
}
