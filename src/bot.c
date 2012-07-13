/* bot.c */

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "bot.h"
#include "logging.h"
#include "main.h"
#include "servers.h"    /* For sockets */

extern struct sys_config *sc;

/* Create and prepare a new bot (memory allocation etc) */

struct Bot * create_bot (void) {

	struct Bot *newbot;

	if ( newbot = (struct Bot *)malloc(sizeof(struct Bot))) {

		/* Allocated ok. Set defaults */

		newbot->list = NULL;
		newbot->nicks = NULL;
		newbot->channels = NULL;
		newbot->current_nick = 0;
		newbot->servers = 0 ;
      newbot->current_server = 0;
      newbot->connected=0;
		newbot->sock = NULL ;
      newbot->port = 6667 ;   /* Default port for IRC */
      newbot->next = NULL;
		newbot->max_connects = 0;
		newbot->botlog[0]='\0';
		newbot->log_motd=0;
		newbot->userid[0] = '\0';
		newbot->description[0] = '\0';
	   newbot->no_nicks=0;
		newbot->no_channels=0;
		newbot->error_connects=0;

      return newbot;

	} else {

		do_sys_log (sc, "Fatal Error: Unable to allocate memory for Bot structure.") ;
      exit_nicely(100);
      
		return newbot;   /* not that this will be called */

   }

}

void kill_bot (struct Bot *killme) {

	/* Add channel/user killing routines */

	struct IRCServer *tempserv;
   struct IRCServer *tempserv2;

	struct Nickname *tempnick;
	struct Nickname *tempnick2;

	/* First, kill the servers */

	tempserv = killme->list;

	while (tempserv) {

		tempserv2 = tempserv;         /* Remember this one... */
		tempserv = tempserv->next;    /* Move on... */
		free (tempserv2);             /* Free memory */

   }

	/* Now, kill the nicknames */

	tempnick = killme->nicks;

	while (tempnick) {

		tempnick2 = tempnick;
		tempnick = tempnick->next;
		free (tempnick2);

	}

   kill_socket (killme) ;         /* Also kill its socket, if it exists */

	/* Now kill the main bot structure */

	free (killme);

}
	
void kill_bots (struct Bots *killme) {

	struct Bot *tempbot;
	struct Bot *tempbot2;

	tempbot = killme->list;

	while (tempbot) {

		tempbot2 = tempbot;         /* Remember... */
		tempbot = tempbot->next;    /* Prepare to move on */
		kill_bot (tempbot2);        /* Call routine to kill that bot */

	}

}
		
/* Put a new bot onto the front of the bot list */
/* Call with first bot on the list */

void insert_bot (struct Bots *b) {

	struct Bot *newbot;	

   newbot = create_bot() ;          /* Does memory allocation etc */
   newbot->next = b->list;          /* Make new bot point to original */
   b->list = newbot;                /* This now goes at the front */
   b->bots++;

}

/* Search through bots, returning details on the selected one */

struct Bot * get_bot (struct Bots *b, int no) {

	struct Bot *temp;
   int count=0;

	for (temp=b->list; temp; temp=temp->next) {

		count++;
		if (count == no) { break; }

	}

   return temp;

}

/* Return number of bots we have initialized */

int no_bots (struct Bots *b) {

	return b->bots;

}

/* Allocate memory for a new IRC Server, and put it on the front of */
/* the IRC Server list */

void insert_IRCServer (struct Bot *b, char *serv, int p) {

	struct IRCServer *n;

   n = (struct IRCServer *)malloc(sizeof(struct IRCServer));
   
   strcpy (n->Server, serv);
   n->port = p ;

   n->next = b->list;    /* Make this point to the next one */
   b->list = n;          /* Put this on the front of the list */
   b->servers++;         /* How many servers... */

}

/* Search through an IRC Server list, returning a pointer to the begining */
/* of the server string, for the required server */

char *get_IRCServer (struct Bot *b, int no) {

	struct IRCServer *temp;
   int count=0;

	for (temp=b->list; temp; temp=temp->next) {

		count++;
		if (count == no) { break; }

	}

   return temp->Server;

}

/* Return the port that a certain IRC server uses */

int get_IRCServerPort (struct Bot *b, int no) {

	struct IRCServer *temp;
	int count=0;

	for (temp=b->list; temp; temp=temp->next) {

		count++;
		if (count == no) { break; }

	}

   return temp->port;

}

/* Return the number of IRC Servers we have defined */

int no_IRCServers (struct Bot *b) {

	return b->servers ;

}

/* Set the server that we are currently dealing with */

void set_IRCServer (struct Bot *b, int no) {

	b->current_server = no;

} 

/* Insert a new nickname onto the front of the nickname list */

void insert_nick (struct Bot *b, char *nick) {

	struct Nickname *n;

   n = (struct Nickname *)malloc(sizeof(struct Nickname));
   
   strcpy (n->nick, nick);

   n->next = b->nicks;   /* Make this point to the next one */
   b->nicks = n;         /* Put this on the front of the list */
   b->no_nicks++;         /* How many servers... */

}	

/* Return the numbered nickname */

char *get_nick (struct Bot *b, int no) {

	struct Nickname *temp;
   int count=0;

	for (temp=b->nicks; temp; temp=temp->next) {

		count++;
		if (count == no) { break; }

	}

   return temp->nick;

}

/* Insert a new channel onto the channel structure list */

void insert_channel (struct Bot *b) {

	struct Channel *n;

   n = (struct Channel *)malloc(sizeof(struct Channel));

   n->userfile[0]='\0';
	n->channel[0]='\0';
	n->logfile[0]='\0';   
	n->no_users=0;
	n->no_cusers=0;
	n->cusers=NULL;
	n->anonops=0;

   n->next = b->channels;    /* Make this point to the next one */
   b->channels = n;          /* Put this on the front of the list */
   b->no_channels++;         /* How many servers... */

}	

/* Return the numbered nickname */

struct Channel * get_channel (struct Bot *b, int no) {

	struct Channel *temp;
   int count=0;

	for (temp=b->channels; temp; temp=temp->next) {

		count++;
		if (count == no) { break; }

	}

   return temp;

}
