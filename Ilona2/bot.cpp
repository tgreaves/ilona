// bot.cpp

#include <iostream.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/socket.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
// #include <clib/socket_inlines.h>

#include "config.h"
#include "bot.h"
#include "logging.h"
#include "main.h"

extern sys_config sc;

// IRCServer class functions

IRCServer::IRCServer (char *serv, IRCServer *item = 0) {
    
    strcpy (Server, serv);           // Create.
    next=item;
    
}

//IRCServer Bot::at_end () {
//
//    if (list == 0) return 0;
//    
//    IRCServer *prev, *curr;
//    prev=curr=list;
//    
//    while (curr) {
//        
//        prev=curr;
//        curr=curr->next;
//    }
//
//    return prev;
//    
//}

void Bot::insertserver (char *val) {
    
    IRCServer *pt = new IRCServer (val, list);
    
    if (pt) {
        
		  list=pt;
        
    } else {
        
        do_sys_log (sc, "Fatal error: Unable to allocate memory for new server entry.");
        
    }

}

void Bot::getserver (int no, char ret[]) {
    
    int count=0;
    
    for (IRCServer *pt=list; pt; pt=pt->next) {
        
        count++;

        if (count==no) { 
            
            break;
            
        }
        
    }
 
    if (count != 0) {
        
        strcpy (ret, pt->Server);
        
    } else {
 
        ret[0]='\0';
        
    }
 
}

// Server stuffs.

void Bot::prepare_socket(void) {
    
    sock = socket (AF_INET, SOCK_STREAM, 0);
    
    if ( sock == ENOBUFS ) {
        
        do_sys_log(sc, "Fatal Error: Couldn't allocate memory for socket.");
        exit_nicely(100);
        
    }

}

// Deal with loading/parsing the configuration
//
// Supported are:
//
//   SERVER

   


