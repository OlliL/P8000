/*
** Includefile zur Steuerung systemabhaengiger Teile in den TQL-Quellen
**
** Es koennen folgende Symbole definiert werden, die in den TQL-Quellen
** genutzt werden:
**
** # define  SUNIX   1	    = fuer UNIX-System
** # define  SPSU    1	    = fuer PSU-Besonderheiten
** # define  SSVP    1	    = fuer SVP 1800-Besonderheiten
** # define  SDCP    1	    = fuer DCP-Besonderheiten
**
** # define  SFSTAT 1	    = fstat() ist vorhanden
**
** v. Treek 19.04.89
*/

# define	TQLSYS	1	/* Includefile-Kenner */
# define	SDCP	1	/* DCP	*/
# define	SFSTAT 1	/* fstat() ist vorhanden */
