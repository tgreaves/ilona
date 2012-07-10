// Test of a main bit

#include <iostream.h>
#include <stdlib.h>            // For exit()
#include <proto/exec.h>
#include <exec/libraries.h>

#include "bot.h"
#include "config.h"
#include "main.h"
#include "logging.h"

sys_config sc;
struct Library *SocketBase = NULL;

void exit_nicely (int nice) {
    
    // To exit safely, shutting down anything we need to.
    
    if (SocketBase) {
        
        CloseLibrary(SocketBase);    // Close bsdsocket.library
        
    }
    
    exit(nice) ;  // For now
   
}

void main (void) {

   char se[100];
   
   sc.set_systemlog("ram:fuck");

   SocketBase = OpenLibrary ( (unsigned char *)"bsdsocket.library",(unsigned long)2);
   
   if (!SocketBase) {
    
      do_sys_log(sc, "Fatal Error: Unable to open bsdsocket.library");      
      exit_nicely(0);

   }

   // System log file check

   do_sys_log(sc, "Library open was successful.");
   
   // Check server lists...
   
   Bot mybot;
   
   mybot.insertserver ("diemen.nl.eu.undernet.org");
   mybot.insertserver ("london.uk.eu.undernet.org");
   
   mybot.getserver(1, se) ;
   cout << se;
   mybot.getserver(2, se) ;
   cout << se;
   
   exit_nicely(0);
   
}
