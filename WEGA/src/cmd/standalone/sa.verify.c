/*
|-----------------------------------------------------------|
|   Standalone-Programm   'sa.verify'    (Version 4.1)      |
|-----------------------------------------------------------|
| Verify mit BTT-Beruecksichtigung                          |
| Bearbeiter: F. Spielmann                                  |
|-----------------------------------------------------------|
*/

char eb[10],db[512];
int btr[40],btr_h[40];
char *ep0,*dp;
char c;
int rg7,rg6,rg5,rg4;
int c_c,kc,dr,cl,hd,sc,lg;
int drs,cls,hds,scs,prk,rmp;
int cyl_b,cyl_e,entrs;
long int zl,zw0,zw1,blks,blocks;
int i,j,k,fa,fb,fc;
long int atol();

main()
{
/* Ausgabe Ueberschrift und Setzen der Anfangswerte */
	printf(">>>  Verify Hard-Disk 4.1  <<<\n");
vfy0:	dp = &db[0];
	ep0 = &eb[0];
	kc = 0x28;
	dr = 0;
	cl = 0;
	hd = 0;
	sc = 1;
	lg = 0x80;

	/* Lesen des Parameterblocks des WDC */
	rg_um();
	c_c = wdc(rg7,rg6,rg5,rg4,dp);
	if (c_c !=0 ) c_c = wdc(rg7,rg6,rg5,rg4,dp);
	if (c_c !=0 ) goto err_h;

	/* Ausgabe der WDC-Kennung */
	db[7] = 0x00;
	printf("\nFirmwareversion '%s'\n",&db[0]);
	if (db[4] < 0x34) if (db[6] < 0x31) {printf("Firmwareversion not ok\n"); goto vfye;}

	/* Umladen und Ausgabe der wesentlichen Parameter */
	db[20] = 0x00;	/* Endekennung fuer printf */
	drs = db[49]&0xff;
	cls = ((db[24] & 0xff) << 8) | (db[23] & 0xff);
	hds = db[25] & 0xff;
	scs = db[26] & 0xff;
	prk = ((db[28]&0xff) << 8) | (db[27]&0xff);
	rmp = db[29]&0xff;
	zw0 = ((db[45] & 0xff) << 8) | (db[44] & 0xff); 
        zw1 = ((db[43] & 0xff) << 8) | (db[42] & 0xff);
	blocks = ((zw0 & 0xffff) * 65536) + (zw1 & 0xffff) + 1;
	printf("PAR --- Drivetype: '%s'\nPAR --- Cylinders: %d  Heads: %d  Sectors: %d  Praecomp: %d  Ramp: %d\n  Number of Drives: %d",&db[8],cls,hds,scs,prk,rmp,drs);
	if (drs == 0 || drs > 2) goto vfye;
	printf(" (Drive");
	for (i=0;i<3;i++) {if (((db[46]&0x07)>>i)&0x01) printf(" %d",i);}
	printf(")");
	printf("  Blocks/Drive: ");
	prt(blocks,10);
	printf("\n\n");

	/* Lesen der Fehlerstatistik und Retten der Fehlerzahlen */
	kc = 0x38;
	lg = 0x80;
	rg_um();
	c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);
	if (c_c != 0) goto err_h;
	fa = (db[16]&0xff)*256+(db[15]&0xff);
	fb = (db[1]&0xff)*256+(db[0]&0xff);
	fc = (db[31]&0xff)*256+(db[30]&0xff);

	/* Abfrage des Laufwerks */
	if (drs != 1)
	{ do
	  { printf("  Drive: ");
	    gets(eb,10);
	    dr=atoi(ep0);
	  } while (dr >= drs);
	}
	else for (i=0;i<3;i++) if (((db[46]&0x07)>>i)&0x01) dr = i;

	/* Lesen der BTT aus dem WDC-RAM in den Host */
	kc = 0x58;
	lg = 40*3+2;
	rg_um();
	c_c = wdc(rg7,rg6,rg5,rg4,dp);
	if (c_c == 0x18) {printf("*** BTT of Drive %d not ok\n",dr); entrs=0;goto b_abfr;}
	if (c_c != 0) goto err_h;

	/* Umwandlung und Ausgabe der BTT */
	entrs = ((db[0] & 0xff) | ((db[1] & 0xff) << 8)) / 3;
	printf("\n  %d Entries in BTT of Drive %d\n",entrs,dr);
	for (i=2,k=0;entrs>0 && k<entrs && k<10;i=i+3,k++)
	{
	  for (j=0;(j*10)+k<entrs;j++)
	  {
            btr[k+(j*10)] = ((db[i+(j*30)] & 0xff) << 8) | (db[i+1+(j*30)] & 0xff); 
	    btr_h[k+(j*10)] = db[i+2+(j*30)] & 0xff;
            printf("    Cyl %d Hd %d",btr[k+(j*10)],btr_h[k+(j*10)]);
	  }
	  printf("\n");
	}

	/* Ausgabe der tatsaechlich verfuegbaren Bloecke */
	blks = blocks - (entrs*scs);
	printf("\nuseful Blocks of Drive %d: ",dr);
	prt(blks,10);
	printf("\n\n");

	/* Abfrage der Spur-Parameter */
b_abfr:
	cyl_b = 0;
	cyl_e = cls-1;
	printf("Verify Begin: Cylinder ");
	gets(eb,10);
	if (eb[0] != 'a')
	{ cyl_b = atoi(ep0);
	  if (cyl_b >= cls) goto b_abfr;
          printf("Verify   End: Cylinder ");
	  gets(eb,10);
	  cyl_e = atoi(ep0);
	  if (cyl_e >= cls || cyl_e < cyl_b) goto b_abfr;
	}

/* Ausfuehrung des Ruecklesens mit Beruecksichtigung der defekten Spuren */
	for (cl = cyl_b;cl <= cyl_e;cl++)
	{
	  printf("   Cylinder %d    \r",cl);
	  kc = 0x44;
	  for (hd=0;hd<hds;hd++)
	  {
	    for (i=0;i<entrs;i++)
	    {
	      if (cl == btr[i] && hd == btr_h[i]) goto sl_hd;
	    }
	    rg_um();
	    c_c = wdc(rg7,rg6,rg5,rg4,dp);
	    if (c_c != 0)
	    {
	      printf("\nError %x --- Kc %x  Dr %d  Cyl %d  Hd %d   continue (y) ?",c_c,kc,dr,cl,hd);
	      gets(eb,10);
	      if (eb[0] != 'y') goto vfye;
	    }
     sl_hd: continue;
	  }
	}
	goto ende;


/* Fehlerauswertung */
err_h:
	printf("\nError %x --- Kc %x\n",c_c,kc);

ende:
	printf("Verify complete      \n");
vfye:	printf("End of 'sa.verify' ? (y/n)");
	gets(eb,10);
	if (eb[0] == 't')
	{ kc = 0x38;
	  lg = 0x80;
	  rg_um();
	  c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);
	  if (c_c != 0) goto err_h;
	  k = (db[16]&0xff)*256+(db[15]&0xff)+(db[1]&0xff)*256+(db[0]&0xff)+(db[31]&0xff)*256+(db[30]&0xff)-fa-fb-fc;
	  printf("   Total errors in 'sa.verify': %d",k);
	  if (k != 0) printf("---> %d * 'A'/%d * 'B'/%d * 'C'",(db[16]&0xff)*256+(db[15]&0xff)-fa,(db[1]&0xff)*256+(db[0]&0xff)-fb,(db[31]&0xff)*256+(db[30]&0xff)-fc);
	  printf("\n");
	  goto vfye;
	}
	if (eb[0] == 'n') goto vfy0;
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

/* UP prt */
prt(lo,b)
register long lo;
register b;
{
  register long a;
  if (a = lo/b) prt(a,b);
  putchar("0123456789ABCDEF"[(int)(lo%b)]);
}
