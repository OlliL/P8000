/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	Standalone	Modul conf.c
 
 
	Bearbeiter:	P. Hoge
	Datum:		04/01/87 11:15:07
	Version:	1.1
 
*******************************************************************************
******************************************************************************/
 

 
#include <sys/param.h>
#include <sys/inode.h>
#include <saio.h>

/*------------------------------------------ Funktionen ---------------------*/
 
 
/*---------------------------------------------------------------------------*/
/*
 *devread
 * Verzweigung zur speziellen Device - Treiber Routine strategy,
 * (Block-Device) mit dem Parameter Read
 * Verzweigung erfolgt ueber devsw am Ende dieses Moduls
 */
/*---------------------------------------------------------------------------*/
devread(io)
register struct iob *io;
{
	return( (*devsw[io->i_ino.i_dev].dv_strategy)(io,READ) );
}

/*---------------------------------------------------------------------------*/
/*
 *devwrite
 * Verzweigung zur speziellen Device - Treiber Routine strategy,
 * (Block-Device) mit dem Parameter Write
 * Verzweigung erfolgt ueber devsw am Ende dieses Moduls
 */
/*---------------------------------------------------------------------------*/
devwrite(io)
register struct iob *io;
{
	return( (*devsw[io->i_ino.i_dev].dv_strategy)(io, WRITE) );
}

/*---------------------------------------------------------------------------*/
/*
 *devopen
 * Verzweigung zur speziellen Device - Treiber Routine open,
 * Verzweigung erfolgt ueber devsw am Ende dieses Moduls
 */
/*---------------------------------------------------------------------------*/
devopen(io)
register struct iob *io;
{
	return( (*devsw[io->i_ino.i_dev].dv_open)(io) );
}

/*---------------------------------------------------------------------------*/
/*
 *devclose
 * Verzweigung zur speziellen Device - Treiber Routine close,
 * Verzweigung erfolgt ueber devsw am Ende dieses Moduls
 */
/*---------------------------------------------------------------------------*/
devclose(io)
register struct iob *io;
{
	return( (*devsw[io->i_ino.i_dev].dv_close)(io) );
}

/*---------------------------------------------------------------------------*/
/*
 *nullsys
 * null Device Treiber routine
 */
/*---------------------------------------------------------------------------*/
nullsys()
{ ; }

/*------------------------------------------ Variablen ----------------------*/
 
/*
 * Device Switch Tabelle
 * Die Tabelle ist ein Feld von n Elementen der Struktur devsw,
 * welche aus jeweils 4 Elementen besteht.
 * Ueber diese Tabelle erfolgt die Verzweigung zu den einzelnen
 * Device Treibern
 * Das 1. Element ist ein Pointer auf den Devicenamen. Dieser wird
 * zB von open hier gesucht.
 * Die folgenden Elemente sind Pointer auf die Device Routinen
 * Das Ende der Tabelle wird mit 0 fuer alle 4 Elemente gekennzeichnet
 
 * Die Tabelle ist Abhaengig von der Implenentation der Devices
 */
 
 
int mdstrategy();
int fdstrategy();
int rmstrat();
int rmopen();
int rmclose();
int udstrat();
int udopen();
int udclose();
int	nullsys();
struct devsw devsw[] =  {
	"md",	mdstrategy,	nullsys,	nullsys,
	"zd",	mdstrategy,	nullsys,	nullsys,
	"fd",	fdstrategy,	nullsys,	nullsys,
	"ud",	udstrat,	udopen,		udclose,
	"rm",	rmstrat,	rmopen,		rmclose,
	0,0,0,0
};
