/*
|----------------------------------------------------|
|    Standalone-Programm   sa.format (Version 4.1)   |
|----------------------------------------------------|
| Programm zum Formatieren von Hard-Disk-Laufwerken  |
| Bearbeiter: F. Spielmann                           |
|----------------------------------------------------|
*/
#define TYP 4

char eb[15];
char bn[13];
char db[512];
unsigned int kc;
unsigned int drs,dr,cls,cl,hds,hd,scs,sc,lgs,lg,prk,rmp;
unsigned int zk0,zk1,zn0,zx0,zn1,zx1;
unsigned int cyl_b,cyl_e,hd_b,hd_e,b_hd,e_hd;
unsigned int i,j,k,tc,entrs;
unsigned int az,cs;
unsigned int c_c;
unsigned int rg7,rg6,rg5,rg4;
struct lw_par {char b[13];
	       unsigned z;
	       unsigned k;
	       unsigned s;
	       unsigned p;
	       unsigned r;
	       unsigned z0;
	       unsigned z1;
	       unsigned n0;
	       unsigned x0;
	       unsigned n1;
	       unsigned x1
	      };
/* Initialisierung der Parameterlisten */
struct lw_par typ[] = {
		{"NEC D5126   ",615,4,18,128,12,203,209,248,250,239,241},
		{"NEC D5146   ",615,8,18,128,12,203,209,248,250,239,241},
		{"ROBOTRON VS ",820,6,18,820,1,203,209,251,253,241,243},
		{"ROB K5504.50",1024,5,18,1024,1,203,209,251,253,241,243},
		      };

main()
{
	/* Initialisierung von Werten */
	tc = 0;		/* tc=0: keine Ausgabe der Zeitkonstanten */
			/* tc=1: Ausgabe der Zeitkonstanten */
	for (i=0;i<12;i++) bn[i] = ' ';
	bn[12] = 0x00;
	cls = 100;
	hds = 2;
	scs = 18;
	prk = cls;
	rmp = 1;
	zk0 = 203; zk1 = 209; zn0 = 251; zx0 = 253; zn1 = 241; zx1 = 243;

	/* Ausgabe der Ueberschrift */
	printf(">>>  Format Hard-Disk 4.1  <<<\n");

	/* Lesen des Parameterblocks des WDC */
ft0:	kc = 0x28;
	dr = 0;
	cl = 0;
	hd = 0;
	sc = 1;
	lg = 0x80;
	rg_um();
	c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);
	if (c_c != 0) c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);
	if (c_c != 0) goto error;

	/* Ausgabe der WDC-Kennung */
	db[7] = 0x00;
	printf("Firmwareversion '%s'\n",&db[0]);
	if (db[4] < 0x34) if (db[6] < 0x31) {printf("Firmwareversion not ok.\n"); goto ft20;}

	/* Ausgabe des Zustandes der angeschlossenen Laufwerke */
	drs = db[49]&0xff;
	printf("Number of Drives: %d",drs);
	if (drs == 0 || drs > 2) goto ft20;
	printf("  (Drive");
	for (i=0;i<3;i++) {if (((db[46]&0x07)>>i)&0x01) printf(" %d",i);}
	printf(")\n");
	if ((db[46]&0x40) != 0x00) printf("Error on RESET\n");
	for (i=0;i<3;i++)
	{ if (((db[46]&0x38)>>(3+i))&0x01) printf(" no CYL0-Signal on Drive %d\n",i);
	}
	for (i=0;i<3;i++)
	{ if (((db[48]&0x07)>>i)&0x01) printf("Cyl 0 Hd 0 Sc 1 of Drive %d not readable\n",i);
	}
	for (i=0;i<3;i++)
	{ if (((db[48]&0x38)>>(3+i))&0x01)
	  { printf("Error in PAR&BTT on Drive %d",i);
	    if (((db[47]>>i)&0x01) != 0x01) printf(" (PAR not ok)");
	    if (((db[47]>>(3+i))&0x01) != 0x01) printf(" (BTT not ok)");
	    printf("\n");
	  }
	}

	/* Abfrage des Laufwerks */
	if (drs != 1)
	{ do
	  { printf("No. of Drive formatted: ");
	    gets(eb,10);
	    dr = atoi(&eb[0]);
	  } while (dr >= drs);
	}
	else 
	for (i=0;i<3;i++) if (((db[46]&0x07)>>i)&0x01) dr = i;

	/* Test des Zustandes der PAR-Tabellen der LW */
	if ((((db[47]&0x7)>>dr)&0x01) == 0x00)
	  if ((db[47]&0x07) == 0x00) goto ft10;

	/* Ausgabe des Inhalts der Parametertabelle */
	for (i=0;i<12;i++) bn[i] = db[8+i];
	cls = ((db[24]&0xff) << 8) | (db[23]&0xff);
	hds = db[25]&0xff;
	scs = db[26]&0xff;
	prk = ((db[28]&0xff) << 8) | (db[27]&0xff);
	rmp = db[29]&0xff;
	zk0 = db[36]&0xff; zk1 = db[37]&0xff;
	zn0 = db[38]&0xff; zx0 = db[39]&0xff;
	zn1 = db[40]&0xff; zx1 = db[41]&0xff;

	/* Ausgabe der Parameter */
ft13:	printf("PAR --- Drivetype: '%s'\nPAR --- Cylinders: %d  Heads: %d  Sectors: %d  Praecomp: %d  Ramp: %d\n",bn,cls,hds,scs,prk,rmp);
	if (tc) printf("PAR --- z40: %d  z41: %d  zmin40: %d  zmax40: %d  zmin41: %d  zmax41: %d\n",zk0,zk1,zn0,zx0,zn1,zx1);

	do
	{ printf("Parameter for Drive ok ? (y/l/p/q)");
	  gets(eb,10);
	  if (eb[0] == 'y') goto ft11;
	  if (eb[0] == 't') {tc=1; goto ft13;}
	  if (eb[0] == 'n') {tc=0; goto ft13;}
	  if (eb[0] == 'q') goto ft20;
	  if (eb[0] == 'p') goto ft12;
	} while (eb[0] != 'l');

/*
----------------------------------------------------------------
| Auswahl von Parametern                                       |
----------------------------------------------------------------
*/
ft10:
	/* Ausgabe der Parametersaetze */
	printf("\nList of Drive-Parameters:");
	printf("\n|No.|  Name      |  cyls; hds; secs; prae; ramp;");
	if (tc) printf(" z40; z41; m40; x40; m41; x41;");
	printf("\n-----------------------------------------------");
	if (tc) printf("-------------------------------");
	for (i=0;i<TYP;i++)
	{ printf("\n| %d |%s|  %d; %d; %d; %d; %d;",i+1,typ[i].b,typ[i].z,typ[i].k,typ[i].s,typ[i].p,typ[i].r);
	  if (tc) printf(" %d; %d; %d; %d; %d; %d;",typ[i].z0,typ[i].z1,typ[i].n0,typ[i].x0,typ[i].n1,typ[i].x1);
	}
	printf("\n-----------------------------------------------");
	if (tc) printf("-------------------------------");

	/* Typauswahl */
	do
	{
       	  printf("\nWhich Typ ? (No./n/q) ");
	  gets(eb,10);
	  if (eb[0] == 'n') goto ft13;
	  if (eb[0] == 'q') goto ft20;
	} while (atoi(eb) < 1 || atoi(eb) > TYP);

	/* Uebertragung der Parameter in die Variablen */
	i = atoi(eb)-1;
	for (k=0;k<12;k++) bn[k] = typ[i].b[k];
	cls = typ[i].z; hds = typ[i].k;
	scs = typ[i].s; prk = typ[i].p; rmp = typ[i].r;
	zk0 = typ[i].z0; zk1 = typ[i].z1;
	zn0 = typ[i].n0; zx0 = typ[i].x0; zn1 = typ[i].n1; zx1 = typ[i].x1;
	goto ft13;

	/* Aenderung von Einzelparametern */
ft12:	printf("Parameters: b(Drivetype(ASCII))\n");
	printf("            c(Number of Cylinders)  h(Number of Heads)\n");
	printf("            s(Number of Sectors)    p(Begin Cylinder of Praecompensation)\n");
	printf("            r(Distance of Cylinders for Ramp Mode)\n");
	if (tc) printf("            z0 z1(Constants for Version of WDC (40 MHz, 41,4 MHz))\n");
	if (tc) printf("            n0 x0 n1 x1(Limits for Version of WDC (40 MHz, 41,4 MHZ))\n");
	printf("            e (End of Input)   q (Jump to the End of 'sa.format')\n");
ft14:	printf("Parameter: (b/c/h/s/p/r)value,e,q ");
	gets(eb,15);
	switch (eb[0])
	{ case 'b' : {for (k=0;(k<13) && (eb[k] != 0x00);k++) bn[k] = eb[k+1];
		      for (;k<13;k++) bn[k-1] = 0x20;
		      bn[12] = 0x00;
		      break;
		     }
	  case 'c' : {cls = atoi(&eb[1]); break;}
	  case 'h' : {hds = atoi(&eb[1]); break;}
	  case 's' : {scs = atoi(&eb[1]); break;}
	  case 'p' : {prk = atoi(&eb[1]); break;}
	  case 'r' : {rmp = atoi(&eb[1]); break;}
	  case 'z' : { switch (eb[1])
		       { case '0' : {zk0 = atoi(&eb[2]); break;}
		         case '1' : {zk1 = atoi(&eb[2]); break;}
		       }
		     }
		     break;
	  case 'n' : { switch (eb[1])
		       { case '0' : {zn0 = atoi(&eb[2]); break;}
		         case '1' : {zn1 = atoi(&eb[2]); break;}
		       }
		     }
		     break;
	  case 'x' : { switch (eb[1])
		       { case '0' : {zx0 = atoi(&eb[2]); break;}
		         case '1' : {zx1 = atoi(&eb[2]); break;}
		       }
		     }
		     break;
	  case 'e' : goto ft13;
	  case 'q' : goto ft20;
	  default  : break;
	}
	goto ft14;

	/* Einordnung der Parameter in den Ausgabebereich */
ft11:	for (i=0;i<12;i++) db[8+i] = bn[i];
	db[20] = db[21] = db[22] = 0x00;
	db[23] = cls&0xff;
	db[24] = ((cls>>8)&0xff)&0xff;
	db[25] = hds;
	db[26] = scs;
	db[27] = prk&0xff;
	db[28] = ((prk>>8)&0xff)&0xff;
	db[29] = rmp;
	for (i=0;i<6;i++) db[30+i] = 0x00;
	db[36] = zk0;
	db[37] = zk1;
	db[38] = zn0;
	db[39] = zx0;
	db[40] = zn1;
	db[41] = zx1;

	/* Schreiben des Parameterblocks auf den WDC-RAM */
	kc = 0x78;
	lg = 34;
	rg_um();
	c_c = wdc(rg7,rg6,rg5,rg4,&db[8]);
	if (c_c != 0x00) goto error;

/*
----------------------------------------------------------------------
| Untersuchung der Lesbarkeit der Spur 0/0                           |
----------------------------------------------------------------------
*/
	/* Spur 0/0 lesbar ? */
	if (db[46]&0x40 == 0x40)
	{ if ((((db[48]&0x7)>>dr)&0x01) == 0x01)
	  { printf("Read Error Cylinder 0, Head 0\n");
	    printf("Format Cylinder 0, Head 0\n");
	    /* Formatieren Zylinder 0, Kopf 0 */
ft15: 	    kc = 0x14;		/*Formatieren mit Ruecklesen*/
	    cl = 0;
	    hd = 0;
	    sc = 1;
	    lg = 3;
	    rg_um();
	    c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);
	    if (c_c != 0)
	    { printf("Format Error %x     repeat ? (y/n)",c_c);
	      do
	      { gets(eb,10);
	        if (eb[0] == 'y') goto ft15;
	        if (eb[0] == 'n') goto ft20;
	      } while ('1');
	    }
	  goto ft8;
	  }
	}

/*
----------------------------------------------------------------------
| Untersuchung der Defektspurtabelle                                 |
----------------------------------------------------------------------
*/
	if ((((db[47]&0x38)>>(3+dr))&0x01) == 0x01) goto ft9;

	/* Loeschen der Defektspurtabelle im WDC-RAM */
ft8:	kc = 0x48;
	rg_um();
	c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);
	if (c_c != 0) goto error;
 
	/* Lesen der WDC-RAM-Defektspurtabelle und Ausgabe */
ft9:	kc = 0x58;		/*BTT-Lesen aus dem WDC-RAM in den Host*/
	lg = 0x7d;		/*Max. Laenge = 40*3 + 2 +3 = 125*/
	rg_um();
	c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);	/*Ausfuehrung*/
	if (c_c != 0) goto error;

	/* Umwandlung und Ausgabe der BTT */
	entrs = ((db[0] & 0xff) | ((db[1] & 0xff) << 8)) / 3;
	if (entrs == 0) printf("  No Entries in Bad Track Table (BTT) of Drive %d\n",dr);
	else
	{ printf("\n  %d Entries in Bad Track Table of Drive %d:\n",entrs,dr);
	  for (i=2,k=0;entrs>0 && k<entrs && k<10;i=i+3,k++)
	  {
	    for (j=0;(j*10)+k<entrs;j++)
	    {
              cs = ((db[i+(j*30)] & 0xff) << 8) | (db[i+1+(j*30)] & 0xff); 
              printf("    Cyl %d Hd %d",cs,db[i+2+(j*30)]&0xff);
	    }
	    printf("\n");
	  }
	}

	/* Abfrage der Handeingabe von Defektspuren */
	do
	{ printf("\nManual Input of bad Track of Drive %d (y/n/q) ? ",dr);
	  gets(eb,10);
	  if (eb[0] == 'n') goto ft2;
	  if (eb[0] == 'q') goto ft20;
	} while (eb[0] != 'y');

	/* Handeingabe von Defektspuren */
ft3:
	do
	{ printf("  Bad Track: Cylinder ");
	  gets(eb,10);
	  if (eb[0] == '\0') goto ft2;
	  cl = atoi(&eb[0]);
	} while (cl >= cls);
	do
	{ printf("                 Head ");
	  gets(eb,10);
	  hd = atoi(&eb[0]);
	} while (hd >= hds);

	/* Einordnen der Defektspur in die vorhandene BTT */
	az = ((db[1]&0xff)<<8) | (db[0]&0xff);
	for (i=2;;i=i+3)
	{
	  cs = ((db[i] & 0xff) << 8) | (db[i+1] & 0xff);
	  if ( cs==cl && (db[i+2]&0xff)==(hd&0xff) ) break;
	  if ( cs>cl || ((cs==cl) && ((db[i+2]&0xff)>(hd&0xff))) )
	  {
	    for (k = az+4; k >= i; k--)
	      db[k+3] = db[k];
	    db[i] = (((cl & 0xff00) >> 8) & 0xff);
	    db[i+1] = cl & 0x00ff;
	    db[i+2] = hd;
	    az = az + 3;
	    db[0] = az & 0x00ff;
	    db[1] = (((az & 0xff00) >> 8) & 0xff);
	    break;
	  }
	}
	goto ft3;

	/* Uebertragen der erzeugten Defektspurtabelle in den WDC-RAM */
ft2:
	printf("\n");
	kc = 0x68;		/*Schreiben der BTT aus dem Host-RAM*/
	lg = 0x7d;
	rg_um();
	c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);	/*Ausfuehrung*/
	if (c_c != 0) goto error;

	/* Abfrage der Spur-Parameter */
ft5:
	cyl_b = 0;
	cyl_e = cls-1;
	b_hd = 0;
	e_hd = hds-1;
	/* Abfrage des Start-Zylinders */
	printf("  Format Begin: Cylinder (a/Start-Cylinder) ");
	gets(eb,10);
	if (eb[0] != 'a' && eb[0] != 'A')
	{ cyl_b = atoi(&eb[0]);
	  if (cyl_b >= cls) goto ft5;
          /* Abfrage des Beginn-Kopfs */
	  do
	  { printf("                    Head ");
	    gets(eb,10);
	    b_hd = atoi(&eb[0]);
	  } while (b_hd >= hds);

          /* Abfrage des End-Zylinders */
	  do
	  { printf("  Format End:   Cylinder ");
	    gets(eb,10);
	    cyl_e = atoi(&eb[0]);
	  } while (cyl_e >= cls || cyl_e < cyl_b);

          /* Abfrage des End-Kopfes */
	  do
	  { printf("                    Head ");
	    gets(eb,10);
	    e_hd = atoi(&eb[0]);
	  } while ((e_hd >= hds) || (cyl_e == cyl_b && e_hd < b_hd));
	}

	/* Ready-Abfrage */
	do
	{ printf("Ready for Format Drive %d from Cyl %d Hd %d\n                           to Cyl %d Hd %d ? (y/n/q) ",dr,cyl_b,b_hd,cyl_e,e_hd);
	  gets(eb,10);
	  if (eb[0] == 'q') goto ft20;
	  if (eb[0] == 'n') goto ft18;
	} while (eb[0] != 'y');

	/* Parameter vorbereiten */
	kc = 0x24;		/*Kommandocode*/
	lg = 3;			/*Laenge einer ev. Ausgabe = 3*/
	for (cl = cyl_b;cl <= cyl_e;cl++)
	{ hd_b = 0;
	  hd_e = hds - 1;
	  if (cl == cyl_b) hd_b = b_hd;
	  if (cl == cyl_e) hd_e = e_hd;
	  for (hd = hd_b;hd <= hd_e;hd++)
	  { printf("      Format Cyl %d  Hd %d\r",cl,hd);
	    rg_um();
	    c_c = wdc(rg7,rg6,rg5,rg4,&eb[0]);
	    if (c_c == 0x21)
	    { cs = ((eb[1] & 0xff) << 8) | (eb[0] & 0xff);
	      printf("      New Entry in BTT: Cyl %d  Hd %d\n",cs,eb[2]&0xff);
	      continue;
	    }
	    if (c_c != 0) goto error;
	  }
	}

	/* Lesen und Ausgabe der Defektspurtabelle aus dem WDC-RAM */
ft18:	kc = 0x58;			/*Lesen der BTT aus dem WDC-RAM*/
	lg = 0x7d;
	rg_um();
	c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);	/*Ausfuehrung*/
	if (c_c != 0) goto error;

	/* Ausgabe der Parameter */
	printf("\nPAR --- Drivetype: '%s'\nPAR --- Cylinders: %d  Heads: %d  Sectors: %d  Praecomp: %d  Ramp: %d\n",bn,cls,hds,scs,prk,rmp);
	if (tc) printf("PAR --- z40: %d  z41: %d  zmin40: %d  zmax40: %d  zmin41: %d  zmax41: %d\n",zk0,zk1,zn0,zx0,zn1,zx1);

	/* Umwandlung und Ausgabe der BTT */
	entrs = ((db[0] & 0xff) | ((db[1] & 0xff) << 8)) / 3;
	if (entrs == 0) printf("  No Entries in Bad Track Table (BTT) of Drive %d\n",dr);
	else
	{ printf("\n  %d Entries in Bad Track Table of Drive %d:\n",entrs,dr);
	  for (i=2,k=0;entrs>0 && k<entrs && k<10;i=i+3,k++)
	  {
	    for (j=0;(j*10)+k<entrs;j++)
	    {
              cs = ((db[i+(j*30)] & 0xff) << 8) | (db[i+1+(j*30)] & 0xff); 
              printf("    Cyl %d Hd %d",cs,db[i+2+(j*30)]&0xff);
	    }
	    printf("\n");
	  }
	}

	/* Schreiben von PAR&BTT aus dem WDC-RAM auf das Laufwerk ? */
	printf("Rewrite PAR&BTT from WDC-RAM to HD-Drive %d ? (y/n)",dr);
	gets(eb,10);
	if (eb[0] != 'y') goto ft20;
	kc = 0xc2;		/*Kommandocode*/
	rg_um();
	c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);
	if (c_c != 0) goto error;

	/* Endeabfrage */
ft20:	printf("\nEnd of 'sa.format' (y/n) ? ");
	gets(eb,10);
	if (eb[0] == 't')
	{ lg = 0x80;
	  rg_um();
	  rg7 = 0x08ca;		/* KC & WDC-Adresse Low */
	  rg6 = 0x3700;		/* WDC-Adresse High */
	  c_c = wdc(rg7,rg6,rg5,rg4,&db[0]);
	  if (c_c != 0) goto error;
	  k = (db[16]&0xff)*256+(db[15]&0xff)+(db[1]&0xff)*256+(db[0]&0xff)+(db[31]&0xff)*256+(db[30]&0xff)+(db[41]&0xff)*256+(db[40]&0xff)+(db[46]&0xff)*256+(db[45]&0xff)+(db[36]&0xff)*256+(db[35]&0xff);
	  printf("   Total Errors since RESET: %d",k);
	  if (k != 0) printf("---> %d * 'A'/%d * 'B'/%d * 'C'/%d * '10'/%d * '11'/%d * '12'",(db[16]&0xff)*256+(db[15]&0xff),(db[1]&0xff)*256+(db[0]&0xff),(db[31]&0xff)*256+(db[30]&0xff),(db[46]&0xff)*256+(db[45]&0xff),(db[36]&0xff)*256+(db[35]&0xff),(db[41]&0xff)*256+(db[40]&0xff));
	  printf("\n");
	  goto ft20;
	}
	if (eb[0] == 'n') goto ft0;
	exit();

/* Fehlerbehandlung */
error:
	printf("  Error %x\n",c_c);
	goto ft20;
}

/* UP rg_um */
rg_um()
{
      rg7 = ((kc & 0x00ff) << 8) | (dr & 0x00ff);
      rg6 = ((cl & 0x00ff) << 8) | ((cl & 0xff00) >> 8);
      rg5 = ((hd & 0x00ff) << 8) | (sc & 0x00ff);
      rg4 = ((lg & 0x00ff) << 8) | ((lg & 0xff00) >> 8);
}
