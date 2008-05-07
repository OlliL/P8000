
/******************************************************************************
*******************************************************************************

	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1988
	KERN	3.2		  Modul: malloc.c

	Bearbeiter:
	Datum:		$D$
	Version:	$R$

*******************************************************************************
******************************************************************************/



#include "sys/param.h"
#include "sys/sysinfo.h"
#include "sys/systm.h"
#include "sys/map.h"


/*--------------------------------------------- Funktionen ------------------*/

/*
 * Die Module malloc und mfree dienen der Speicherverwaltung in den
 * beiden Speichern coremap und swapmap
 */

/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
/*
 * malloc
 * Bereitstellung von freiem Speicher aus der core- bzw. swapmap
 * Zuweisung von 'size' Einheiten 
 * Rueckgabe Basis des zugewiesenen Bereiches
 * In der map werden die Adressen erhoeht, die Liste wird durch eine
 * 0-Laenge beendet
 * Coremap-Unit = 64 Byte; Swapmap-Unit = 512 Byte
 * Algorithmus ist first-fit.
 */
/*---------------------------------------------------------------------------*/
malloc(mp, size)
struct map *mp;
{
	register unsigned int a;
	register struct map *bp;

	for (bp=mp; bp->m_size; bp++) {
		if (bp->m_size >= size) {
			a = bp->m_addr;
			bp->m_addr += size;
			if ((bp->m_size -= size) == 0) {
				do {
					bp++;
					(bp-1)->m_addr = bp->m_addr;
				} while ((bp-1)->m_size = bp->m_size);
			}
			return(a);
		}
	}
	return(0);
}

/*---------------------------------------------------------------------------*/
/*
 * mfree
 * Freigabe des vorher zugewiesenen Speichers a mit der Laenge von size
 * Einheiten an die spezifizierte Map
 * 'a' wird in die Map einsortiert und ein oder beide Enden kombiniert,
 * wenn moeglich
 */
/*---------------------------------------------------------------------------*/
mfree(mp, size, a)
struct map *mp;
register int a;
{
	register struct map *bp;
	register unsigned int t;

	if ((bp = mp)==coremap && runin) {
		runin = 0;
		wakeup((caddr_t)&runin);
	}
	for (; bp->m_addr<=a && bp->m_size!=0; bp++);
	if (bp>mp && (bp-1)->m_addr+(bp-1)->m_size == a) {
		(bp-1)->m_size += size;
		if (a+size == bp->m_addr) {
			(bp-1)->m_size += bp->m_size;
			while (bp->m_size) {
				bp++;
				(bp-1)->m_addr = bp->m_addr;
				(bp-1)->m_size = bp->m_size;
			}
		}
	} else {
		if (a+size == bp->m_addr && bp->m_size) {
			bp->m_addr -= size;
			bp->m_size += size;
		} else if (size) {
			do {
				t = bp->m_addr;
				bp->m_addr = a;
				a = t;
				t = bp->m_size;
				bp->m_size = size;
				bp++;
			} while (size = t);
		}
	}
}
