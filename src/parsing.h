/* Ilona - parsing.h */

#ifndef PARSING_H
#define PARSING_H

char *GetToken (char *, char *, char *);
struct User * find_user (struct Bot *, char *);
void parse_ping (struct Bot *, char *);
void parse_001 (struct Bot *, char *);
void parse_353 (struct Bot *, char *);
void parse_372 (struct Bot *, char *);
void parse_433 (struct Bot *, char *);
void parse_473 (struct Bot *, char *);
void parse_join (struct Bot *, char *);
void parse_privmsg (struct Bot *, char *);
void parse_error (struct Bot *, char *);
void parse_irc (struct Bot *);

void ilona_help (struct Bot *, struct User *, char *);
void ilona_op (struct Bot *, struct User *, char *);
void ilona_quit (struct Bot *, struct User *, char *);
void ilona_raw (struct Bot *, struct User *, char *);
void ilona_showchanusers (struct Bot *, struct User *,char *);
void ilona_showcommands (struct Bot *, struct User *, char *);

/* irctab */
/* Structure to hold IRC protocol commands */

struct irctab
{
    char  *Name;
    void (*Func)(struct Bot *, char *);
} ;

/* ilonatab */
/* Structure to hold Ilona's commands */

struct ilonatab
{
    char *Name;
    int level;
    void (*Func)(struct Bot *, struct User *, char *);
} ;

#endif

