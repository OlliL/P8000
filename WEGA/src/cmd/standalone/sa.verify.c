/* Verify mit BTT-Beruecksichtigung */

#define CUR_UP '\013'

  char ebuf[10],dbuf[512];
  int btr[40],btr_h[40];
  char *ep0,*dp;
  char c;
  int p_cnt,z_cnt;
  int rg7,rg6,rg5,rg4;
  int c_c;
  int kc,drv,cyl,head,sec;
  int drs,cyls,heads,secs,lg,wadd,wlen;
  int cyl_b,cyl_e,entrs;
  long int zl,zw0,zw1,block,blocks;
  int i,i1,j,k;
  long int atol();

main()
{
/* Ausgabe Ueberschrift und Setzen der Anfangswerte */
	printf("**** Verify V. 2.0 ****\n");
	dp = &dbuf[0];
/* Uebernahme der WDC-PROM-Parameter */
	rg7 = 0x2800;
	rg4 = 0x0002;
	c_c = wdc(rg7,rg6,rg5,rg4,dp);
	if (c_c !=0 ) c_c = wdc(rg7,rg6,rg5,rg4,dp);
	if (c_c !=0 ) goto err_h;
/* Umladen und Ausgabe der wesentlichen Parameter */
	drs = dbuf[12] & 0xff;
	dbuf[12] = 0x00;	/* Endekennung fuer printf */
	cyls = ((dbuf[14] & 0xff) << 8) | (dbuf[13] & 0xff);
	heads = dbuf[15] & 0xff;
	secs = dbuf[16] & 0xff;
	lg = ((dbuf[18] & 0xff) << 8) | (dbuf[17] & 0xff);
	zw0 = ((dbuf[24] & 0xff) << 8) | (dbuf[23] & 0xff); 
        zw1 = ((dbuf[22] & 0xff) << 8) | (dbuf[21] & 0xff);
	blocks = ((zw0 & 0xffff) * 65536) + (zw1 & 0xffff) + 1;
	printf("  WDC-Firmware-Version: '%s'\n  Drives: %d    Cylinders: %d    Heads: %d\n  Sectors: %d    Bytes/Sector: %d\n  Blocks/Drive: ",dbuf,drs,cyls,heads,secs,lg);
	prt(blocks,10);
	printf("\n\n");

/* Abfrage des Laufwerks */
driv:
	ep0 = &ebuf[0];
	printf("  Drive: ");
	gets(ebuf,10);
	drv = atoi(ep0);
	if (drv >= drs) {printf("  ?\n"); goto driv;}

/* Lesen der BTT aus dem WDC-RAM in den Host */
/*	kc = 0x58; */
	kc = 0x08;
	rg7 = 0x0800;
	rg6 = 0x3600;
	dp = &dbuf[0];
	lg = 40*3+2;
	rg4 = 0x7a00;
/*	rg_um1(); */
	c_c = wdc(rg7,rg6,rg5,rg4,dp);
	if (c_c != 0) goto err_h;

/* Umwandlung und Ausgabe der BTT */
	entrs = ((dbuf[0] & 0xff) | ((dbuf[1] & 0xff) << 8)) / 3;
	printf("  %d Entries in BTT of Drive %d\n",entrs,drv);
	for (i=2,k=0;entrs>0 && k<entrs;i=i+3,k++)
	{
	  btr[k] = ((dbuf[i] & 0xff) << 8) | (dbuf[i+1] & 0xff); 
	  btr_h[k] = dbuf[i+2] & 0xff;
          printf("    Cyl %d  Head %d\n",btr[k],btr_h[k]);
	}

/* Abfrage der Spur-Parameter */
b_abfr:
	cyl_b = 0;
	cyl_e = cyls-1;
	printf("  Begin: Cylinder ");
	gets(ebuf,10);
	if (ebuf[0] != 'a' && ebuf[0] != 'A')
	{ cyl_b = atoi(ep0);
	  if (cyl_b >= cyls)
	    { printf("  ?\n");
	      goto b_abfr;
	    }

          printf("  End:   Cylinder ");
	  gets(ebuf,10);
	  cyl_e = atoi(ep0);
	  if (cyl_e >= cyls || cyl_e < cyl_b)
	  { printf("  ?\n");
	    goto b_abfr;
       	  }
	}

/* Ausfuehrung des Ruecklesens mit Beruecksichtigung der defekten Spuren */
	for (cyl = cyl_b;cyl <= cyl_e;cyl++)
	{
	  printf("   Cylinder %d    \r",cyl);
	  kc = 0x44;
	  for (head=0;head<heads;head++)
	  {
	    for (i=0;i<entrs;i++)
	    {
	      if (cyl == btr[i] && head == btr_h[i]) goto sl_hd;
	    }
	    rg_um1();
	    c_c = wdc(rg7,rg6,rg5,rg4,dp);
	    if (c_c != 0)
	    {
	      printf("\nError %x --- Kc %x  Dr %d  Cy %d  Hd %d   continue ?",c_c,kc,drv,cyl,head);
	      gets(ebuf,10);
	      if (ebuf[0] != 'y' && ebuf[0] != 'Y') goto ende;
	    }
     sl_hd: continue;
	  }
	}
	goto ende;


/* Fehlerauswertung */
err_h:
	printf("\nError %x --- Kc %x\n",c_c,kc);

ende:
	printf("Verify complete\n");
	exit();
}



/* UP rg_um1 */
rg_um1()
{
      rg7 = ((kc & 0x00ff) << 8) | (drv & 0x00ff);
      rg6 = ((cyl & 0x00ff) << 8) | ((cyl & 0xff00) >> 8);
      rg5 = ((head & 0x00ff) << 8) | (sec & 0x00ff);
      rg4 = ((lg & 0x00ff) << 8) | ((lg & 0xff00) >> 8);
}

/* UP rg_um2 */
rg_um2()
{
      rg7 = ((kc & 0x00ff) << 8) | (drv & 0x00ff);
      rg6 = ((cyl & 0x00ff) << 8) | ((cyl & 0xff00) >> 8);
      rg5 = ((head & 0x00ff) << 8) | (sec & 0x00ff);
}

/* UP rg_um_bl */
rg_um_bl()
{
      rg7 = ((kc & 0x00ff) << 8) | (drv & 0x00ff);
      rg6 = ((block & 0x000000ff) << 8) | ((block & 0x0000ff00) >> 8);
      rg5 = ((block & 0x00ff0000) >> 8) | ((block & 0xff000000) >> 24);
      rg4 = ((lg & 0x00ff) << 8) | ((lg & 0xff00) >> 8);
}

/* UP atox */
atox(s)
char *s;
{
	unsigned int n;
	for (n = 0;(*s >= '0' && *s <= '9') || (*s >= 'a' && *s <='f');s++)
	{
	  n = 16 * n + *s - '0';
	  if (*s >= 'a') n = n - 0x27;
	}
	return(n);
}

prt(lo,b)
register long lo;
register b;
{
  register long a;
  if (a = lo/b) prt(a,b);
  putchar("0123456789ABCDEF"[(int)(lo%b)]);
}

