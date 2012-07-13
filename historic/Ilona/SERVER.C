
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

/* Server routines */
/* Included by main.c */

void get_host_stuff( char input[] ) {

   hp = gethostbyname (input) ;

   if (hp == NULL) {

      #ifdef AMIGA
      MUI_RequestA(0,0,0,"Error","Ok","Unknown host %s", input) ;
      #else
      printf("Failed: Unknown host %s\n",input) ;
      #endif

      do_sys_log("Unknown host %s", input) ;
      exit_nicely (100) ;
   }

}

void prepare_socket(void) {

   my_socket = socket (AF_INET, SOCK_STREAM, 0) ;

   if ( my_socket == ENOBUFS ) {

      printf("\nError: Couldn't allocate memory for socket.\n") ;
      do_sys_log("Error: Couldn't allocate memory for socket.") ;

      exit_nicely(100) ;

   }

}

void SendToServer ( char *format, ...) {

   char buffer [512] ;
   va_list msg;

   va_start(msg, format) ;
   vsprintf(buffer, format, msg) ;
   va_end(msg);

   strcat(buffer, "\r\n") ;      /* Signify end of message */

   if ( send (my_socket, buffer, strlen(buffer), 0 ) == -1 ) {

      /* Error occured during sending to server */

      do_sys_log("Error: Couldn't send to server.") ;
      printf("Error: Couldn't send to server.") ;

      exit_nicely(100) ;

   }

}

int grab_server_line ( char output[] ) {

   int keep_going, location, temp ;

   char holding [1] ;

   keep_going = 1 ;

   location = 0 ;

   while (keep_going == 1) {

      temp = recv (my_socket, holding, 1, 0) ;

      if (temp == -1) {

         /* Error reading */

         #ifdef AMIGA
         MUI_RequestA(0,0,0,"Error","Ok","Couldn't read from socket.", 0) ;
         #else
         printf("Error: Couldn't read from socket.") ;
         #endif

         do_sys_log ("Error: Couldn't read from socket.") ;
         exit_nicely (100) ;

      }

      /* We don't want the \r and \n end of line characters */

      if (strncmp ("\r", holding, 1) == 0 ) {

         /* End of msg. Read one more char to flush */

         temp = recv (my_socket, holding, 1, 0);  /* Will be \n */

         output [location] = '\0' ;

         keep_going = 0 ;
        
      } else {

         /* Is a character we want, so store it... */

         output [location] = holding [0] ;

         location++ ;

      }

   }

   location = location - 1 ; /* So accurate length is returned. */

   return location ;

}

void do_connect(void) {

   int temp = 0 ;

   do_sys_log("Attempting connection to %s (port %i).", hp->h_name, sport) ;

   max_connects-- ;

   #ifdef AMIGA
   DoMethod (App->TX_label_0, MUIM_Set, MUIA_Text_Contents, "\033cConnecting to server") ;
   #else
   printf("Connecting to %s (port %d)....\n",hp->h_name, sport) ;
   #endif

   /* First copy all the data we need to the server structure */

   bzero ( (char*)&server, sizeof (server) ) ;
   server.sin_family = hp->h_addrtype ;
   server.sin_port = (unsigned short)htons(sport) ;
   memcpy( (char *)&server.sin_addr, hp->h_addr, hp->h_length) ;

   /* Now attempt the connection */

   temp = connect (my_socket, (struct sockaddr *)&server, sizeof (server)) ;

   if (temp !=0) {

      #ifdef AMIGA
      MUI_RequestA(0,0,0,"Error","Ok","Connection could not be established.", 0) ;
      #else
      printf("Connection could not be established.\n\n") ;
      #endif

      do_sys_log("Connection could not be established.") ;

      exit_nicely (100) ;
   }

   #ifdef AMIGA
   DoMethod (App->TX_label_0, MUIM_Set, MUIA_Text_Contents, "\033cHandshaking") ;
   #else
   printf("Connected.\n") ;
   #endif

   do_sys_log("Connected! Sending handshake.") ;

   /* Now we need to send our startup information */

   if ( servpasswd[0] != '\0' ) {

      /* Server password defined, so send it */

      do_sys_log("Password connection, using %s", servpasswd) ;

      SendToServer ("PASS %s", servpasswd) ;

   }

   SendToServer ("NICK %s", nickname) ;

   SendToServer ("USER %s . . :%s", username, realname) ;

}

