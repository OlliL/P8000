/*
|------------------------------------------------------|
|      Standalone-Programm  sa.shipdisk (Version 4.1)  |
|------------------------------------------------------|
| Programm zum Bewegen des Schreib-Lese-Kamms          |
| auf den innersten Zylinder fuer alle Laufwerke       |
| Bearbeiter: F. Spielmann                             |
|------------------------------------------------------|
*/

char db[512];
unsigned int kc,dr,drs,cl,hd,sc,lg;
int i;
unsigned int c_c;
unsigned int rg7,rg6,rg5,rg4;

main()
{
	printf("\n>>> Shipdisk 4.1 <<<\n");

/* Lesen des Parameterblocks */
	kc = 0x28;
	lg = 0x40;
	rg_um();
	c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);
	if (c_c != 0) c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);
	if (c_c != 0) {printf("Error %x\n",c_c); exit();}

/* Lesen auf dem innersten Zylinder */
	drs = db[49]&0xff;
	cl = (((db[24]&0xff) << 8) | (db[23]&0xff)) - 1;
	kc = 0x01;
	hd = 0;
	sc = 1;
	lg = 0x200;
/* Leseversuch auf jedem angeschlossenem Laufwerk */
	for (dr=0;dr<drs;dr++)
	{ rg_um();
	  c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);
	}
	printf("\nShipdisk ok.\n");
	exit();
}

/* UP rg_um */
rg_um()
{
      rg7 = ((kc & 0x00ff) << 8) | (dr & 0x00ff);
      rg6 = ((cl & 0x00ff) << 8) | ((cl & 0xff00) >> 8);
      rg5 = ((hd & 0x00ff) << 8) | (sc & 0x00ff);
      rg4 = ((lg & 0x00ff) << 8) | ((lg & 0xff00) >> 8);
}
