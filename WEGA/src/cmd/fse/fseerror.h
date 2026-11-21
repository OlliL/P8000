#define MAXERR 10            /* maximale Fehleranzahl */
#define ERRFILE "errmsg"

struct errlist {
unsigned int line;
unsigned int column;
char         text[76];      /* endet mit '\0', dass heisst 75 Zeichen Text */
};
