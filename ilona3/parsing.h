/* Ilona - parsing.h */

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

struct irctab {
   char  *Name;
   void (*Func)(struct Bot *, char *);
} ;

/* Next we fill that with the commands and their functions */

struct irctab irctable[] = {

   {"PING", parse_ping },
   {"001", parse_001 },
   {"353", parse_353 },
   {"372", parse_372 },
	{"433", parse_433 },
	{"473", parse_473 },
	{"JOIN", parse_join },
	{"PRIVMSG", parse_privmsg },
	{"ERROR", parse_error },
   {NULL, NULL}
   
};

/* ilonatab */
/* Structure to hold Ilona's commands */

struct ilonatab {
	char *Name;
	int level;
	void (*Func)(struct Bot *, struct User *, char *);
} ;

/* Now fill the commands in */

/* Level 10   -   RAW, QUIT  */

struct ilonatab ilonatable[] = {

	{"OP", 2, ilona_op },
	{"HELP", 1, ilona_help },
	{"QUIT", 10, ilona_quit },
	{"RAW", 10, ilona_raw },
	{"SHOWCHANUSERS", 10, ilona_showchanusers},
	{"SHOWCOMMANDS", 1, ilona_showcommands},
	{NULL, NULL}
	
};
