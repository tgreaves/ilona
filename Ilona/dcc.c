
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

/* DCC Server Routines */

int prepare_chat_socket(int the_sock) {

   chat_socket[the_sock] = socket (AF_INET, SOCK_STREAM, 0) ;

   if ( chat_socket[the_sock] == ENOBUFS ) {

      do_sys_log("Error: Couldn't allocate memory for DCC Chat socket (Number %d)", the_sock) ;
      return 0 ;

   } else {

      return 1 ;  /* Everything peachy */

   }
 
}

void SendToChat ( int the_sock, char *format, ...) {

   char buffer [512] ;
   va_list msg;

   va_start(msg, format) ;
   vsprintf(buffer, format, msg) ;
   va_end(msg);

   strcat(buffer, "\r\n") ;

   if ( send (chat_socket[the_sock], buffer, strlen(buffer), 0 ) == -1 ) {

      /* Error occured during sending to server */

      do_sys_log("Error: Couldn't send to DCC Chat socket! (Number %d)", the_sock) ;
      printf("Error: Couldn't send to DCC Chat socket!") ;

      exit_nicely(20) ;

   }

}

int grab_chat_line ( int the_sock, char output[] ) {

   char gcl_buf [512] ;

   int keep_going, location, temp ;

   keep_going = 1 ;

   location = 0 ;

   temp = recv (chat_socket[the_sock], gcl_buf, sizeof(gcl_buf), 0) ;

   if (temp == -1) {

      /* Error reading */
      /* No need to panic. Just wipe out the connection */

      do_sys_log("DCC Port %d: Connection lost.", the_sock) ;
      
      #ifdef AMIGA
      CloseSocket (chat_socket[the_sock]) ;
      #else
      close (chat_socket[the_sock]) ;
      #endif
      chat_socket[the_sock] = NULL ;
      return 0 ;
 

   }

   location = temp ;    /* Number of bytes */
   gcl_buf[location-1] = '\0' ;
   
   strcpy (output, gcl_buf) ;

   return location ;

}

void process_chat_line ( int the_sock, char cl[] ) {

    char *bufptr, *command ;
    int recognised ;

    /* This gets passed the chat line */
    /* Do one command... quit */

    bufptr = cl ;
    recognised = 0 ;

    command = GetToken (&bufptr, " ", " ") ;

    if (stricmp ("HELP", command) == 0) {

       recognised=1;
       SendToChat(the_sock, "The following Ilona file server commands are available...") ;
       SendToChat(the_sock, "help, quit") ;

    }

    if (stricmp ("QUIT", command) == 0) {

        recognised=1;
        SendToChat(the_sock, "Thank you for using this file server.") ;

        #ifdef AMIGA
        CloseSocket (chat_socket[the_sock]) ;
        #else
        close (chat_socket[the_sock]) ;
        #endif
        chat_socket[the_sock]=NULL ;

    }

    if (recognised==0) {

        SendToChat(the_sock,"Unrecognised command. Please type HELP for a list.") ;

    }

}


int do_chat_connect(int the_sock) {

   int temp = 0 ;
   int port_conv ;
   unsigned long host_conv ;
   char *tail ;

   /* UNDER DEVELOPMENT */

   /* DCC CHAT format:   DCC Chat 12345678 1234567 */
   /*                             ADDRESS    PORT  */

   /* These are previously grabbed and stored in strings */
   /* chat_host and chat_port, respectively */

   /* We need to convert these strings into the right types */

   host_conv = strtol (chat_host, &tail, 0) ;
   port_conv = strtol (chat_port, &tail, 0) ;

   do_sys_log("DCC Port %d: Attempting DCC Chat connection to %s (port %s)", the_sock, chat_host, chat_port) ;

   /* Our socket is already open and available at this point */

   bzero ( (char*)&chat_server[the_sock], sizeof (chat_server[the_sock]) ) ;  /* Clear it */

   chat_server[the_sock].sin_port = (unsigned short)htons(port_conv) ;
   chat_server[the_sock].sin_family = AF_INET ;
   chat_server[the_sock].sin_addr.s_addr = (unsigned long)htonl(host_conv) ; 

   /* Now attempt the connection */

   temp = connect (chat_socket[the_sock], (struct sockaddr *)&chat_server[the_sock], sizeof (chat_server[the_sock])) ;

   if (temp !=0) {

      #ifdef AMIGA
      do_sys_log("DCC Port %d: DCC Chat could not be established (Error code %d)", the_sock, Errno() ) ;
      #else
      do_sys_log("DCC Port %d: DCC Chat could not be established (Error code %d)", the_sock) ;
      #endif

      #ifdef AMIGA
      CloseSocket (chat_socket[the_sock]) ;
      #else
      close (chat_socket[the_sock]) ;
      #endif
      chat_socket[the_sock] = NULL ;

      return 1 ;

   }

   do_sys_log("DCC Port %d: DCC Chat connection was established.", the_sock) ;
   return 0 ;

}

