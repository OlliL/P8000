/*----------------------------------------------------*/
/*     Standalone-Programm   sa.format                */
/*----------------------------------------------------*/
/* Programm zum Formatieren eines Hard-Disk-Laufwerks */
/*----------------------------------------------------*/

char buf[10] ;
char dat_buf[512] ;
main()
{ unsigned int kc;
  unsigned int drs,cyls,hds,secs,lg;
  unsigned int cyl_b,cyl_e,hd_b,hd_e,b_hd,e_hd;
  unsigned int cyl,head;
  unsigned int i,k;
  unsigned int az,cs,cl,he;
  unsigned int drive;
  long int blks;
  unsigned int c_c;
  unsigned int rg7,rg6,rg5,rg4;
  char *db;
  char *pb;

	printf("\n*** Format Hard-Disk V. 1.4 ***\n");
	rg7 = 0x2800;
	rg4 = 0x0002;
	db = &dat_buf[0];
	pb = &buf[0];
	c_c = wdc(rg7,rg6,rg5,rg4,db);
	if (c_c != 0) c_c = wdc(rg7,rg6,rg5,rg4,db);
	if (c_c != 0) goto error;

/* Umladen der wesentlichen Parameter */
	drs = dat_buf[12] & 0xff;
	dat_buf[12] = 0x00;
	cyls = ((dat_buf[14] & 0xff) << 8) | (dat_buf[13] & 0xff);
	hds = dat_buf[15] & 0xff;
	secs = dat_buf[16] & 0xff;
	lg = ((dat_buf[18] & 0xff) << 8) | (dat_buf[17] & 0xff);
	cs = dat_buf[24] & 0xff;
	blks = cs * 256;
	cs = dat_buf[23];
	cs = cs & 0x00ff;
	blks = blks + cs;
	blks = blks * 256;
	cs = dat_buf[22];
	cs = cs & 0x00ff;
	blks = blks + cs;
	blks = blks * 256;
	cs = dat_buf[21];
	cs = cs & 0x00ff;
	blks = blks + cs;
	blks = blks + 1; 

/* Ausgabe der Parameter */
	printf("  WDC-Firmware-Version: '%s'\n  Drives: %d    Cylinders: %d    Heads: %d\n  Sectors: %d    Bytes/Sector: %d\n  Blocks/Drive: %D\n\n",dat_buf,drs,cyls,hds,secs,lg,blks);

/* Abfrage des Laufwerks */
/*
	do
	{ printf("  Drive: ");
	  gets(buf,10);
	  drive = atoi(pb);
	} while (drive >= drs);
*/
	drive = 0;

/* Abfrage, ob die BTT vom Laufwerk eingelesen werden soll */
ft1:
	printf("  Read BTT from HD in WDC-RAM ? ");
	gets(buf,10);
	if (buf[0] != 'y' && buf[0] != 'Y') goto ft8;
/* Einlesen der BTT vom Laufwerk */
	rg7 = 0x4100;
	c_c = wdc(rg7,rg6,rg5,rg4,db);
	if (c_c == 0) goto ft9;
	if (c_c == 6) 
	{ printf("  BTT not ok.\n");
	  goto ft8;
	}
	printf("  Read Error Cylinder 0, Head 0\n");
	printf("  Format Cylinder 0, Head 0 ? ");
	gets(buf,10);
	if (buf[0] != 'y' && buf[0] != 'Y') goto ft1;
/* Formatieren Zylinder 0, Kopf 0 */
	rg7 = 0x2400;
	rg6 = 0x0000;
	rg5 = 0x0001;
	rg4 = 0x0300;
	c_c = wdc(rg7,rg6,rg5,rg4,db);
	if (c_c != 0) goto error;

ft8:
/* Loeschen der Defektspurtabelle im WDC-RAM */
	rg7 = 0x4800;
	c_c = wdc(rg7,rg6,rg5,rg4,db);
	if (c_c != 0) goto error;
 
ft9:
/* Lesen der WDC-RAM-Defektspurtabelle und Ausgabe */
	rg7 = 0x0800;		/*WDC-RAM-Lesen & Laufwerk*/
	rg6 = 0x3600;		/*WDC-RAM-Adresse der BTT*/
	rg5 = 0x0000;
	rg4 = 0x8000;		/*Anzahl der Bytes*/
	c_c = wdc(rg7,rg6,rg5,rg4,db);	/*Ausfuehrung*/
	if (c_c != 0) goto error;
	if (dat_buf[0] == 0 && dat_buf[1] == 0)
	  printf("  No Entries in Bad Track Table (BTT)\n");
	else
	{ printf("      Entries in Bad Track Table:\n");
	  for(i = 2;dat_buf[i] != 0xff;i = i+3)
	  {
	    cs = ((dat_buf[i] & 0xff) << 8) | (dat_buf[i+1] & 0xff);
	    printf("      Cyl %d  Hd %d\n",cs,dat_buf[i+2] & 0xff);
	  }
	}

/* Abfrage der Handeingabe von Defektspuren */
	printf("\n  Manual Input of Bad Track ? ");
	gets(buf,10);
	if (buf[0] != 'y' && buf[0] != 'Y') goto ft2;
/* Handeingabe von Defektspuren */
ft3:
	do
	{ printf("  Bad Track: Cylinder ");
	  gets(buf,10);
	  if (buf[0] == '\0') goto ft2;
	  cl = atoi(pb);
	} while (cl >= cyls);
	do
	{ printf("                 Head ");
	  gets(buf,10);
	  he = atoi(pb);
	} while (he >= hds);
/* Einordnen der Defektspur in die vorhandene BTT */
	az = ((dat_buf[1]&0xff)<<8) | (dat_buf[0]&0xff);
	for (i=2;;i=i+3)
	{
	  cs = ((dat_buf[i] & 0xff) << 8) | (dat_buf[i+1] & 0xff);
	  if ( cs==cl && (dat_buf[i+2]&0xff)==(he&0xff) ) break;
	  if ( cs>cl || ((cs==cl) && ((dat_buf[i+2]&0xff)>(he&0xff))) )
	  {
	    for (k = az+4; k >= i; k--)
	      dat_buf[k+3] = dat_buf[k];
	    dat_buf[i] = (((cl & 0xff00) >> 8) & 0xff);
	    dat_buf[i+1] = cl & 0x00ff;
	    dat_buf[i+2] = he;
	    az = az + 3;
	    dat_buf[0] = az & 0x00ff;
	    dat_buf[1] = (((az & 0xff00) >> 8) & 0xff);
	    break;
	  }
	}
	goto ft3;

/* Uebertragen der durch Handeingabe erzeugten Defektspurtabelle in den WDC-RAM */
ft2:
	printf("\n");
	rg7 = 0x1800;		/*WDC-RAM-Schreiben*/
	rg6 = 0x3600;		/*WDC-RAM-Adresse der BTT*/
	rg5 = 0x0000;
	rg4 = 0x8000;		/*Anzahl der Bytes*/
	c_c = wdc(rg7,rg6,rg5,rg4,db);	/*Ausfuehrung*/
	if (c_c != 0) goto error;

/* Abfrage der Spur-Parameter */
ft5:
	cyl_b = 0;
	cyl_e = cyls-1;
	b_hd = 0;
	e_hd = hds-1;
	/* Abfrage des Start-Zylinders */
	printf("  Format Begin: Cylinder ");
	gets(buf,10);
	if (buf[0] != 'a' && buf[0] != 'A')
	{ cyl_b = atoi(pb);
	  if (cyl_b >= cyls) goto ft5;
          /* Abfrage des Beginn-Kopfs */
	  do
	  { printf("                    Head ");
	    gets(buf,10);
	    b_hd = atoi(pb);
	  } while (b_hd >= hds);

          /* Abfrage des End-Zylinders */
	  do
	  { printf("  Format End:   Cylinder ");
	    gets(buf,10);
	    cyl_e = atoi(pb);
	  } while (cyl_e >= cyls || cyl_e < cyl_b);

          /* Abfrage des End-Kopfes */
	  do
	  { printf("                    Head ");
	    gets(buf,10);
	    e_hd = atoi(pb);
	  } while ((e_hd >= hds) || (cyl_e == cyl_b && e_hd < b_hd));
	}

/* Ready-Abfrage */
	printf("  Ready for format from Cyl %d Hd %d to Cyl %d Hd %d : ",cyl_b,b_hd,cyl_e,e_hd);
	gets(buf,10);
	if (buf[0] != 'y' && buf[0] != 'Y') exit();

/* Parameter vorbereiten */
	rg7 = 0x2400;		/*Kommandocode & Laufwerk*/
	rg4 = 0x0300;		/*Laenge einer ev. Ausgabe = 3*/
	for (cyl = cyl_b;cyl <= cyl_e;cyl++)
	{ hd_b = 0;
	  hd_e = hds - 1;
	  if (cyl == cyl_b) hd_b = b_hd;
	  if (cyl == cyl_e) hd_e = e_hd;
	  for (head = hd_b;head <= hd_e;head++)
	  { printf("      Format Cyl %d  Hd %d\r",cyl,head);
	    rg6 = cyl & 0x00ff;
	    rg6 = rg6 << 8;
	    rg5 = cyl & 0xff00;
	    rg5 = rg5 >> 8;
	    rg5 = rg5 & 0x00ff;
	    rg6 = rg6 | rg5;
	    rg5 = head & 0x00ff;
	    rg5 = rg5 << 8;
	    c_c = wdc(rg7,rg6,rg5,rg4,pb);
	    if (c_c == 0x21)
	    { cs = ((buf[1] & 0xff) << 8) | (buf[0] & 0xff);
	      printf("      New Entry in BTT: Cyl %d  Hd %d\n",cs,buf[2]&0xff);
	      continue;
	    }
	    if (c_c != 0) goto error;
	  }
	}

/* Lesen und Ausgabe der Defektspurtabelle aus dem WC-RAM */
	rg7 = 0x0800;		/*WDC-RAM-Lesen & Laufwerk*/
	rg6 = 0x3600;		/*WDC-RAM-Adresse der BTT*/
	rg5 = 0x0000;
	rg4 = 0x8000;		/*Anzahl der Bytes*/
	c_c = wdc(rg7,rg6,rg5,rg4,db);	/*Ausfuehrung*/
	if (c_c != 0) goto error;
	if (dat_buf[0] == 0 && dat_buf[1] == 0)
	  printf("  No Entries in Bad Track Table (BTT)\n");
	else
	{ printf("\n    Entries in Bad Track Table:\n");
	  for(i = 2;dat_buf[i] != 0xff;i = i+3)
	  {
	    cs = ((dat_buf[i] & 0xff) << 8) | (dat_buf[i+1] & 0xff);
	    printf("      Cyl %d  Hd %d\n",cs,dat_buf[i+2] & 0xff);
	  }
	}

/* Schreiben der BTT aus dem WDC-RAM auf das Laufwerk ? */
	printf("  Rewrite BTT from WDC-RAM to HD ? ");
	gets(buf,10);
	if (buf[0] != 'y' && buf[0] != 'Y') exit();
	rg7 = 0xc200;		/*Kommandocode & Laufwerk*/
	c_c = wdc(rg7,rg6,rg5,rg4,db);
	if (c_c != 0) goto error;
	exit();

/* Fehlerbehandlung */
error:
	printf("  Error %x\n",c_c);
	exit();
}

