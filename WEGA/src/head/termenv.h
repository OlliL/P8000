/* termenv.h */

/* change EOF to NULL in te fopen of TTYTYPES to eliminate warning msg
 *   --ejm 3/26/81
 */
#define TTYTYPES "/etc/ttytype"

char *termenv = "TERM=__" ;

int _same ( str1, str2 )
char *str1, *str2 ;
{
 for ( ; *str1 == *str2; str1++, str2++ )   /* bop along strings     */
    if ( *str1 == '\0' ) return (1) ;       /* both must be '\0'     */
 return (0) ;                               /* no match              */
}


char *_ttytype ( name )
char *name ;
{
 FILE *fd ;
 char tcode[3], dname[20] ;

 if ( (fd = fopen ( TTYTYPES, "r" )) != NULL )
   {
    while ( fscanf ( fd, "%s%s", tcode, dname ) != EOF )
        if  ( _same ( name, dname ) )
	{
	    fclose(fd);
            return ( tcode ) ;
   }
   }
    else printf("stypeof: _ttytype: can't open %s", TTYTYPES) ;

    fclose(fd);
    return ( "un" ) ;       /* this is surely an error condition */
}


char *stypeof ( ttyid )                     /* return ptr to string of form */
char *ttyid;                                /* TERM=xx where xx is 2 char   */
{                                           /* code from /etc/ttytype       */
 char *ptr, *tcode ;

 for ( ptr=ttyid ; *ptr != '\0'; ptr++ ) ;    /* get to end of path name     */
 for ( --ptr; *ptr != '/' ; --ptr ) ;         /* get to last / in path       */
 ptr++ ;

 tcode = _ttytype ( ptr ) ;             /* get 2 char code form /etc/ttytype */
 termenv[5] = *tcode++ ;                      /* first letter of term code   */
 termenv[6] = *tcode   ;                      /* second letter of the code   */
}
