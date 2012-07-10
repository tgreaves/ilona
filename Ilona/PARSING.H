/* LogBot Header File */

/* For parsing.c */

/* Declare what we have in the file */

#ifndef PARSING_H

#define PARSING_H

/* Text parsing routines */

char *GetToken(char **source, char *starttokens, char *endtokens) ;
void strip_backs (char *text) ;

/* Identification parsing */

void GetNick (char *dest, char *nuh) ;
int ConfirmID (char *from) ;
int find_user (char *usertofind) ;

/* And the nasty big-bast of a function */

void parse_line ( char buffer[] ) ;

#endif /* PARSING_H */
