
 /***************************************************************************/
 /*                               Ilona v1.2                                */
 /*-------------------------------------------------------------------------*/
 /*                  (c) Copyright 1997 by Tristan Greaves                  */
 /*                           All rights reserved                           */
 /*                                                                         */
 /*    The material within is an unpublished copyrighted work containing    */
 /*    trade secrets or otherwise sensitive information. This copy has      */
 /*    been provided on the basis of strict confidentiality and on the      */
 /*    express understanding that it may not be reproduced or revealed      */
 /*    to any person, in whole or in part, without express written per-     */
 /*    mission from Tristan Greaves, who is the sole owner of copyright     */
 /*    and all other rights therein.                                        */
 /*                                                                         */
 /***************************************************************************/

/* The Main File */

#include <stdio.h>                             /* IO */
#include <stdlib.h>                            /* Exit() */
#include <stdarg.h>                            /* Lists */
#include <string.h>                            /* Strings */
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>                        /* Socket Library */
#include <sys/errno.h>                         /* Socket Library Error Codes */
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>

#include "main.h"
#include "config.h"
#include "server.h"
#include "parsing.h"
#include "logging.h"
#include "dcc.h"

#define DISABLESTUFF

#ifdef AMIGA

#include "rexx.h"

#include <exec/libraries.h>
#include <proto/exec.h>
#include <clib/socket_inlines.h>

/* MUI includes */

#include <clib/muimaster_protos.h>
#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>

#include <pragmas/muimaster_pragmas.h>
#include <pragmas/exec_pragmas.h>

#include "Ilona_MUI.c"

struct Library *MUIMasterBase ;
struct Library *SocketBase ;

struct ObjApp * App = NULL ;  /* MUI App */

int gui_activated ;

#endif /* AMIGA */

#define NO_CHATS 5

struct hostent *hp ;

struct in_addr addr;
struct sockaddr_in server ;
struct sockaddr_in chat_server[NO_CHATS] ;

int my_socket, joins, parts, nomess ;  /* Socket and statistics */
int sport, chat_socket[NO_CHATS] ;

char log_day [3] ;         /* Storage for log start day */
char systemlog [100] ;
char channellog [100] ;
char userfile [100] ;
char commandfile [100] ;
char server_name [40] ;    /* Server name */
char channel [15]     ;    /* Channel name */
char nickname [15]    ;    /* Bot nickname to use */
char servpasswd [15]  ;    /* Server pw to use */
char username [20] ;       /* WHOIS Username */
char realname [40]  ;      /* WHOIS Realname */
int  rejoin_kick ;         /* 1 = Join on kick, 0 = Don't! */
int  connected ;           /* Connected to server? */
int  logging ;             /* 1 = Logging channel, 0 = Not logging channel */
int  invisible ;           /* 1 = Make bot invisible, 0 = Don't */
int  log_archive ;         /* 1 = 24 hr archive, 0 = Don't */
int  log_stamp ;           /* 0 = No, 1 = Stamp time, 2 = Stamp d/t */
int  log_stats ;           /* At end of log, record stats? */
int  html_log ;            /* 1 = Enable HTML log, 0 = Don't */
int  html_protect ;        /* 1 = HTML protection, 0 = None */
int  max_connects ;        /* Attempts to re-connect to a server (D: 5) */
int  max_users ;           /* Part of keyfile mechanism */
int  reggie ;
int  in_channel ;
char LBREGI[100] ;         /* Storage for Registration details */
char MYREGI[100] ;         /* So that I can find out full details */

int no_users ;
char user_id[50][20] ;
char user_host[50][40] ;
char user_status[50][10] ;
char user_oper[50][10] ;
char user_greet[50][200] ;
int found;                 /* Keep a record of user no. after a check */

int no_coms ;
char com_name[150][20] ;
int com_level[150] ;
char com_type[150][5] ;
char com_text[150][500] ;

char com_char[2] ;

char chat_host [30] ;
char chat_port [10] ;

char html_header [100] ;   /* HTML header filename */

#ifdef AMIGA

#define LBVERS "Ilona v1.4 (AmigaOS). (c) Copyright 1997 Tristan Greaves."

#else

#define LBVERS "Ilona v1.4 (Linux). (c) Copyright 1997 Tristan Greaves."

#endif

/* Following defines required for true portability */

#define FALSE 0
#define TRUE 1

#ifndef AMIGA

#define stricmp(s1,s2) strcasecmp(s1,s2)

#endif

/* ---------------------------------------------------------------- */

#include "config.c"
#include "server.c"
#include "parsing.c"
#include "logging.c"
#include "dcc.c"

#ifdef AMIGA

#include "rexx.c"

#endif

/* ---------------------------------------------------------------- */

#ifdef AMIGA

void open_libs(void) {

   SocketBase = OpenLibrary("bsdsocket.library", 2) ;

   if (SocketBase == NULL) {

      printf("Error: Couldn't open bsdsocket.library!\n") ;
      printf("Have you got AmiTCP running?\n") ;

      exit(100) ;

   }

   MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN) ;

   if (MUIMasterBase == NULL) {

      printf("Error: Couldn't open muimaster.library\n") ;
      CloseLibrary(SocketBase) ;
      exit (100) ;

   }

}

void close_libs(void) {

    if (SocketBase) {

      CloseLibrary(SocketBase) ;
      SocketBase = NULL ;

    }

   if (MUIMasterBase) {

      CloseLibrary(MUIMasterBase) ;
      MUIMasterBase = NULL ;

   }

}

#endif /* AMIGA */

void exit_nicely (int ec) {

   int count ;

   if (my_socket) {

      #ifdef AMIGA   

      CloseSocket (my_socket) ;
      
      #else

      close (my_socket) ;

      #endif /* AMIGA */

   }

   for (count=0; count<NO_CHATS; count++) {

      if (chat_socket) {

         #ifdef AMIGA
         CloseSocket (chat_socket[count]) ;
         #else
         close (chat_socket[count]) ;
         #endif /* AMIGA */
         chat_socket[count] = NULL ;

      }

   }

   #ifdef AMIGA

   if (gui_activated==1) { DisposeApp(App) ; }
   close_libs() ;

   #endif /* AMIGA */

   do_sys_log ("Ilona v1.4 closed.\n") ;
   exit (ec) ;

}

void SigInt (int sig) {

   /* Called when user requests a break */
   /* Note, this doesn't exit "cleanly", from an IRC server's view */

   do_sys_log("User break detected.") ;

   if (connected==1) {

      do_sys_log ("There were %i joins and %i parts.", joins, parts) ;
      do_sys_log ("There were %i public messages.", nomess) ;

      SendToServer("QUIT :Local shutdown.") ;

   }

   exit_nicely(0) ;  

}

void keyfile_handler (void) {

   FILE *keyfile ;
   char Key_Details[10] ;
   char Key_ID[10] ;
   char Key_Name[50] ;
   char Key_Surname[50] ;
   char key_version[5] ;
   char checkstr[10] ;
   char regstr[200] ;

   char *tail ;
   int i, leng, checkit ;
   int mycheck = 0 ;
   int kf_done=0;

   checkit=0 ;

   /* Sort out keyfile stuff */
   /* ILKEY Technology */

   checkit=0 ;

   keyfile = fopen ("ilona.key", "r") ;

   if (keyfile != 0) {

      fscanf(keyfile, "%s %s %s %s %s %s", Key_Details, key_version, Key_ID, Key_Name, Key_Surname, checkstr) ;

      fclose (keyfile) ;

   } else {

      /* Un registered */

      #ifdef AMIGA
      DoMethod (App->TX_label_1, MUIM_Set, MUIA_Text_Contents, "\033c(c) Copyright 1997 Tristan Greaves\n\n*** Unregistered Version ***\n\nThis is a MUI Application\nMUI is copyrighted by Stefan Stuntz") ;
      #endif

      max_users = 3 ;
      reggie = 0;
      kf_done=1 ;

      strcpy(LBREGI, "** Unregistered Version **") ;

   }

   /* Need to convert checkstr to an integer */

   if (kf_done != 1) {

      checkit = strtol (checkstr, &tail, 0) ;
   
      /* Confirm that this *IS* an Ilona Key */

      if ( strcmp ("ILKEY", Key_Details) != 0 ) {

         #ifdef AMIGA
         MUI_RequestA(0,0,0,"Error","Ok","Your Ilona key is not valid. Please contact the author.", 0) ;
         #else
         printf("Error: Your Ilona key is not valid. Please contact the author.\n") ;
         #endif
         exit_nicely(100) ; ;

      }

      if ( strcmp ("12", key_version) != 0 ) {

         #ifdef AMIGA
         MUI_RequestA(0,0,0,"Error","Ok","Your Ilona key is not compatable with this version of Ilona.\nPlease contact the author.", 0) ;
         #else
         printf("Error: Your Ilona key is not compatable with this version of Ilona.\nPlease ocntact the author.\n") ;
         #endif
         exit_nicely(100) ; ;

      }

      leng = strlen (Key_ID) ;

      for (i=0; i<leng; i++) {

         Key_ID[i]=Key_ID[i] - (4 * (i+1) ) ;    /* Reverse encryption */
         mycheck=mycheck + ( Key_ID[i] + 4 ) ;

      }

      /* Right, now to do the same thing with the Key_Name */

      leng = strlen (Key_Name) ;

      for (i=0; i<leng; i++) {

         Key_Name[i] = Key_Name[i] - (6 * (i+1)) ;  /* Reverse... */
         mycheck=mycheck + ( Key_Name[i] + 6 ) ;

      }

      /* And surname */

      leng = strlen (Key_Surname) ;

      for (i=0; i<leng; i++) {

         Key_Surname[i] = Key_Surname[i] - (3 * (i+1)) ;  /* Reverse... */
         mycheck=mycheck + ( Key_Surname[i] + 3 ) ;

      }

      if (mycheck != checkit) {

         #ifdef AMIGA
         MUI_RequestA(0,0,0,"Error","Ok","Your Ilona key is not valid. Please contact the author.", 0) ;
         #else
         printf("Error: Your Ilona key is not valid. Please contact the author.\n") ;
         #endif
         exit_nicely(100) ; ;

      }

      /* At this point, key file handling is finished */
      /* And it is valid */
 
      #ifdef AMIGA
      sprintf(regstr, "\033c(c) Copyright 1997 Tristan Greaves\n\nRegistered to: %s %s (#%s)\n\nThis is a MUI Application\nMUI is copyrighted by Stefan Stuntz", Key_Name, Key_Surname, Key_ID) ;
      DoMethod (App->TX_label_1, MUIM_Set, MUIA_Text_Contents, regstr) ;
      #endif

      max_users = 50 ;
      reggie = 1 ;

      /* Also need to build LBREGI at this point */
      /* Altered for security */

      sprintf(MYREGI, "Registered To: %s %s (#%s)", Key_Name, Key_Surname, Key_ID) ;
      sprintf(LBREGI, "Registered To #%s.", Key_ID) ;

   }

}

void main(int argc, char * argv[] ) {

   int nb, count, temp2 ;
   struct timeval timer ;
   fd_set rd ;
   char buffer [512] ;
   long date ;
   char *temp ;
   
   #ifdef AMIGA

   BOOL running = TRUE ;
   ULONG signal ;

   open_libs() ;
   App = CreateApp() ;
   gui_activated = 1 ;

   #endif /* AMIGA */

   timer.tv_sec  = 1 ;     /* Old value was 40 */
   timer.tv_usec = 0 ;

   #ifndef AMIGA
   printf("%s\n", LBVERS) ;
   #endif

   keyfile_handler() ;

   #ifndef AMIGA
   printf("%s\n\n",MYREGI) ;  /* Registered text */
   #endif

   if (argc == 1) {

      /* Use default config file */

      #ifdef AMIGA

         grab_config("ilona.cfg") ;   /* Amiga paths */

      #else

         grab_config("ilona.cfg") ;    /* Unix paths */

      #endif   /* AMIGA */

   } else {

      if (argc == 0) {

         /* Has been started from Amiga workbench */

         grab_config ("PROGDIR:ilona.cfg") ;   /* Zero length filename */

      } else {

         /* Another one specified */

         grab_config( argv[1] ) ;

      }

   }

   grab_userfile () ;

   if ( commandfile[0] != '\0' ) { grab_comfile() ; }

   /* Store opening date number (For log file archiving */

   date = time (NULL) ;

   temp = DateToString (date) ;

   strncpy (log_day, temp, 2) ;

   do_sys_log("Ilona v1.4 opened.") ;

   /* Check if max users exceeded */

   if ( (no_users+1) > max_users ) {

      if (reggie==1) {

         #ifdef AMIGA
         MUI_RequestA(0,0,0,"Error","Ok","Maximum number of users exceeded (Max=20)", 0) ;
         #else
         printf("Error: Maximum number of users exceeded (Max=20)\n") ;
         #endif

      } else {

         #ifdef AMIGA
         MUI_RequestA(0,0,0,"Error","Ok","Maximum number of users exceeded.\nFor more users, please register this software!", 0) ;
         #else
         printf("Error: Maximum number of users exceeded.\nFor more users, please register this software!\n") ;
         #endif

      }

      do_sys_log("Error: Maximum number of users exceeded.") ;
      exit_nicely(100) ;

   }

   do_sys_log("Looking for host %s", server_name ) ;

   #ifdef AMIGA
   DoMethod (App->TX_label_0, MUIM_Set, MUIA_Text_Contents, "\033cLooking for host") ;
   #else
   printf("Looking for host %s\n", server_name) ;
   #endif

   joins = 0 ;

   parts = 0 ;

   nomess = 0 ;    /* Set stats */

   /* We need to set up our traps for CTRL C as well */
   /* (Only on Linux - Handled by MUI on Amiga */

#ifndef AMIGA
   signal (SIGINT, SigInt) ; 
#endif

   get_host_stuff( server_name ) ;

   prepare_socket() ;

   do_connect() ;

   /* Amiga version checks for MUI exit calls. Linux does not. */
   
#ifdef AMIGA
   while (running) {
#else
   while (1) {
#endif

#ifdef AMIGA

      switch (DoMethod(App->App,MUIM_Application_Input,&signal)) {

         case MUIV_Application_ReturnID_Quit:

            running=FALSE ;
            break ;

         case 1:

            do_sys_log("Success\n") ;
            break ;

      }

#endif /* AMIGA */

      FD_ZERO(&rd) ;

      FD_SET(my_socket, &rd) ;

      /* Need to see which chats to monitor */

      for (count=0; count<NO_CHATS; count ++) {
       
         if (chat_socket[count]) {

            /* If DCC Chat monitoring is required... */

            FD_SET(chat_socket[count], &rd) ;

         }

      }

      nb = select (FD_SETSIZE, &rd, NULL, NULL, &timer) ;

      if (nb < 0) {

        /* Ignore timeouts */

        #ifdef AMIGA
        do_sys_log("Wait() failure... Errno=%d", Errno() ) ;
        #else
        do_sys_log("Wait() failure for sockets.") ;
        #endif

      }

      if (FD_ISSET (my_socket, &rd) ) {

         /* IRC Server input */

         grab_server_line ( buffer ) ;
         parse_line (buffer ) ;

      }

      for (count=0; count<NO_CHATS; count++) {

         if (chat_socket[count]) {

            if (FD_ISSET (chat_socket[count], &rd)) {

               /* DCC Chat Connection input */

               temp2 = grab_chat_line (count, buffer) ;         

               if ( temp2 != 0 ) { process_chat_line (count, buffer) ; }
           
            }

         }

      }

   }

   exit_nicely(0) ;  /* Handles socket shutdown + exit message */

}

