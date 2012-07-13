/* bot.h */
/* Header file for an individual Ilona bot. Remember, several "Ilonas" can be controlled */
/* from a single instance of the program. */

#ifndef BOT_H

#define BOT_H

/* IRCServer structure */

struct IRCServer
{

    char Server[100];               /* Server name. */
    int port;                       /* Where to connect */
    struct IRCServer *next;         /* Pointer to the next one. */

};

/* Nickname structure */

struct Nickname
{

    char nick[10];							/* IRC nickname */
    struct Nickname *next;           /* Pointer to the next one */

} ;

/* Channel structure */

struct Channel
{

    struct ChanUser *cusers;         /* Pointer to the channel user structure */
    struct User *users;              /* Pointer to the filed user structure */

    char channel[50];						/* Full channel name */
    char userfile[100];					/* Filename of the channel's user file */
    char logfile[100];					/* Filename of the channel log file */
    struct Channel *next;				/* Pointer to the next one */

    int no_users;							/* Number of users in the main user file */
    int no_cusers;							/* Number of users in channel user file */

    int anonops;							/* 0 = tell who ops, 1 = don't */

} ;

/* Bot structure (for one instance of Ilona */

struct Bot
{

    struct IRCServer *list;         /* Pointer to the IRC Server list. */
    struct Nickname *nicks;         /* Pointer to the nickname list */
    struct Channel *channels;       /* Pointer to the channel list */

    char userid [20];               /* User ID */
    char description [100];         /* Bot description */

    int servers;                    /* Number of servers in the list */
    int current_server;             /* The server we are currently dealing with */
    int connected;                  /* 0 = Not connected, 1 = Connected to IRC server */
    int max_connects;               /* How many times to attempt a re-connection */
    int error_connects;					/* On a single server, no. of ERRORS */

    int no_nicks;                   /* Number of nicknames */
    int current_nick;	            /* Current nick in use */

    int no_channels;						/* Number of channels */

    struct hostent *hp;             /* gethostbyname() lookup info */
    struct sockaddr_in *server;     /* Internet details of host */
    int port;                       /* Latest port number */
    int sock;                       /* TCP/IP socket. */
    char buffer [512] ;             /* Current buffer */
    char lastfrom [100];            /* Userhost of last command issuer */
    char botlog [100] ;             /* Bot log file */

    int log_motd;                   /* 0 = Don't, 1 = Log MOTD on connect */

    struct Bot *next ;              /* Pointer to the next bot */

} ;

/* Main bots structure - linked list */

struct Bots
{

    struct Bot *list ;               /* Pointer to our first bot */
    int bots;                        /* The number we have */

} ;

struct Bot * create_bot (void);                  /* Prepapre a new bot structure */
void kill_bot (struct Bot *);                    /* Destroy a bot's structure/memory */

void insert_bot (struct Bots *b);                /* Insert a new bot, is created too */
void kill_bots (struct Bots *b);                 /* Kill all bots */
struct Bot * get_bot (struct Bots *, int);       /* Return pointer to requested bot */
int no_bots (struct Bots *);                     /* Return number of bots */

void insert_IRCServer (struct Bot *, char *, int); /* Insert an IRC server. */
char *get_IRCServer (struct Bot *, int);         /* Return the numbered server. */
int get_IRCServerPort (struct Bot *, int);       /* Return port that a server uses */
int no_IRCServers (struct Bot *);                /* Return number of IRC Servers */
void set_IRCServer (struct Bot *, int);          /* Set IRC server to use */

void insert_nick (struct Bot *, char *);         /* Insert a nickname */
char *get_nick (struct Bot *, int);              /* Return a nickname */

void insert_channel (struct Bot *);              /* Insert a channel */
struct Channel * get_channel (struct Bot *, int);           /* Return a channel */

#endif /* BOT_H */
