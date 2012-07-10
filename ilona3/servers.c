/* Deals with Internet servers */

/* #include <sys/types.h> */
/* #include <sys/socket.h> */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <proto/socket.h>
#include <netdb.h>
#include <sys/errno.h>
/* #include <pragmas/socket_pragmas.h> */
#include "tg_socket_lib.h"   /* My fixed socket pragmas for StormC */

#include "servers.h"
#include "config.h"
#include "bot.h"
#include "logging.h"
#include "main.h"

extern struct sys_config *sc;

/* Prepare the hostent structure for a bot for a specified server */
/* The server will have been defined in the bot already */

int get_host_stuff (struct Bot *b, int no) {

	struct IRCServer *temp;
	int count=0;
	
	for (temp=b->list; temp; temp=temp->next) {
	
		count++;
		if (count == no) {break; }
		
	}

   do_bot_log(b, "Looking for %s...", temp->Server);
	
	b->hp = gethostbyname (temp->Server);
	
	if (b->hp == NULL) {
	
		do_bot_log(b, "Server look-up failure: Unknown host %s", temp->Server);
		return FALSE; 
		
	} else {
	
	   b->port = temp->port;   /* Set latest port number */
		return TRUE;
		
	}
	
}

void prepare_socket (struct Bot *b) {
	
	int temp;
	
	temp = socket (AF_INET, SOCK_STREAM, 0) ;
	
	if (temp != ENOBUFS) {
	
		/* We got a socket. Put it in the bot structure */
		
		b->sock = temp;
		
	} else {
	
		do_sys_log(sc, "Fatal Error: Unable to allocate memory for Internet socket.");
		exit_nicely(100);
		
	}
	
}

/* Kill socket. This is also called by kill_bot */

void kill_socket (struct Bot *b) {

	if (b->sock != NULL) {
	
		CloseSocket (b->sock) ;
		
   }
   
}

/* Attempt to connect a bot to IRC server */
/* IRC server has already been looked up at this point */

int do_connect (struct Bot *b) {

   struct sockaddr_in server;

	do_bot_log(b, "Attempting connection to %s (port %i)", b->hp->h_name, b->port);

   /* Copy the details we need to the Internet structure. */
   /* We create a new server structure first. */

   /* server = malloc(sizeof(struct sockaddr_in)); */

	bzero ( (char*)&server, sizeof(server) ) ;  
   server.sin_port = (unsigned short)htons(b->port);
	memcpy ( (char *)&server.sin_addr, b->hp->h_addr, b->hp->h_length);
	server.sin_family = b->hp->h_addrtype;
      
	b->server = &server;       /* Point from bot structure */
	
	/* Now we can reference all of this from the bot structure */
	
	if ( connect (b->sock, (struct sockaddr *)&server, sizeof (server)) == 0) {
	
		do_bot_log(b, "Connected - Sending handshake.");
      b->connected = 1 ;

		b->current_nick = b->no_nicks;    /* Use first choice nickname */

      SendToServer (b, "NICK %s", get_nick (b, b->current_nick)) ;
      SendToServer (b, "USER %s . . :%s", b->userid, b->description) ;

		return TRUE;
		
	} else {
	
		do_bot_log(b, "Connection could not be established.");
		return FALSE;
	
   }

}

void SendToServer (struct Bot *b, char *format, ...) {

	char buffer [512] ;
	
	va_list msg;
	va_start (msg, format);
   vsprintf (buffer, format, msg) ;
	va_end(msg);
	
	strcat (buffer, "\r\n");        /* Standard way to end IRC command */
	
	if ( send (b->sock, buffer, strlen(buffer),0) == -1 ) {
	
		/* Error occured */
		/* Assume this server has died... */
		
		do_bot_log(b, "Error: Unable to send to server. Assuming server has died.");
		kill_socket (b);
		b->connected = 0 ;
		
	}
	
}

/* Read a line of input from the IRC Server */
/* We have already been told that data is waiting for us */

void grab_server_line (struct Bot *b) {

	int keep_going, location, temp;
	char holding[1];
	char output[512];
	
	output[0]='\0';
	keep_going = 1;
	location = 0;
	
	while (keep_going == 1) {
	
		temp = recv (b->sock, holding, 1, 0) ;
		
		if (temp == -1) {
		
			/* Error reading from socket */
			/* Assume server has died */
			
			do_bot_log(b,"Error: Unable to read from socket. Assuming server has died.");
			kill_socket (b);
			b->connected=0;
			keep_going=0;
			break;
			
		}

		if (temp == 0) { keep_going = 0; }   /* End of data - premature? */
		
		/* We don't want the \r and \n end of line characters */
		
		if (strncmp ("\r", holding, 1) == 0) {
		
			/* End of message - read one more char to flush */
			
			temp = recv (b->sock, holding, 1, 0);
			output [location] = '\0';
			keep_going = 0;
			
		} else {
		
			/* Is a normal character, so store it */
			
			output [location] = holding[0];
			location++;
			
		}
		
	}

   strcpy (b->buffer, output);
	
}