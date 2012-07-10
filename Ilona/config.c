
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

/* Configuration Handling Routines */
/* Included by main.c */

void grab_config (char cfgname[]) {

   char string [81] ;

   char setting[20], argument[40] ;

   char *tail ;
   char *handle;

   FILE *cfgfile ;

   systemlog[0]='\0' ;
   channellog[0]='\0' ;
   commandfile[0]='\0' ;
   server_name[0]='\0' ;
   nickname[0]='\0' ;
   channel[0]='\0' ;
   servpasswd[0]='\0' ;
   username[0]='\0' ;
   realname[0]='\0' ;
   html_header[0]='\0' ;
   userfile[0]='\0' ;
   com_char[0]='\0' ;

   rejoin_kick = 1 ;    /* Default: Always rejoin on a kick */
   html_log = 0 ;       /* Default: Standard logging */
   html_protect = 1 ;   /* Default: HTML protected */
   invisible = 0 ;      /* Default: Bot is not invisible */
   log_archive = 0 ;    /* Default: Don't do archiving */
   log_stamp = 0 ;      /* Default: No stamping */
   log_stats = 0 ;      /* Default: No stats */
   max_connects = 5 ;   /* Default: Try 5 times */

   /* Attempt to load cfg */

   cfgfile = fopen (cfgname, "r") ;

   if ( cfgfile != 0) {

      /* File found...load and parse */
      /* Only load one line for the moment */

      while ( fgets (string, (int) sizeof(string)-1, cfgfile) !=0 ) {

         sscanf (string, "%s %s", &setting, &argument) ;

         /* Test for what argument it is */

         if ( stricmp ("SYSTEM_LOG", setting) == 0 ) {

            strcpy (systemlog, argument) ; /* Set system log file */

         }

         if ( stricmp ("CHANNEL_LOG", setting) == 0 ) {

            strcpy (channellog, argument) ; /* Set channel log file */

         }
 
         if ( stricmp ("USERFILE", setting) == 0 ) {

            strcpy (userfile, argument) ; /* Set userfile */

         }

         if ( stricmp ("COMMAND_FILE", setting) == 0) {

            strcpy (commandfile, argument) ;

         }

         if ( stricmp ("COMMAND_CHAR", setting) == 0) {
 
            strcpy (com_char, argument) ;

         }

         if ( stricmp ("LOG_ARCHIVE", setting) == 0 ) {

            log_archive = strtol (argument, &tail, 0) ;

         }
 
         if ( stricmp ("LOG_STAMP", setting) == 0) {

            log_stamp = strtol (argument, &tail, 0) ;

         }

         if ( stricmp ("LOG_STATS", setting) == 0) {

            log_stats = strtol (argument, &tail, 0) ;

         }

         if ( stricmp ("HTML_LOG", setting) == 0) {

            html_log = strtol (argument, &tail, 0) ;

         }

         if ( stricmp ("HTML_HEADER", setting) == 0) {

            strcpy (html_header, argument) ; /* Set HTML header filename */

         }

         if ( stricmp ("HTML_PROTECT", setting) == 0) {
 
            html_protect = strtol (argument, &tail, 0) ;

         }

         if ( stricmp ("USERNAME", setting) == 0) {

            strcpy (username, argument) ; /* Set WHOIS user name */
         }

         if ( stricmp ("REALNAME", setting) == 0) {

            /* strcpy (realname, argument) ;  */

            handle=string ;  /* Start of config line */
            handle=handle+9 ;
            strcpy (realname, handle) ;

         }

         if ( stricmp ("SERVER", setting) == 0) {

            strcpy (server_name, argument) ;  /* Set server name */

         }

         if ( stricmp ("PORT", setting) == 0) {

            sport = strtol (argument, &tail, 0) ;

         }

         if ( stricmp ("SERVER_PASSWORD", setting) == 0) {

            strcpy (servpasswd, argument) ; /* Server password */

         }

         if ( stricmp ("MAX_CONNECTS", setting) == 0) {

            max_connects = strtol (argument, &tail, 0 ) ;

         }

         if ( stricmp ("NICK", setting) == 0) {

            strcpy (nickname, argument) ;  /* Set bot nickname */

         }

         if ( stricmp ("CHANNEL", setting) == 0) {

            strcpy (channel, argument) ;  /* Set channel to join */

         }

         if ( stricmp ("REJOIN_KICK", setting) == 0) {

            rejoin_kick = strtol (argument, &tail, 0) ;

         }

         if ( stricmp ("INVISIBLE", setting) == 0) {

            invisible = strtol (argument, &tail, 0) ;

         }

      }

      fclose (cfgfile) ;
      
      /* Check required values are here */
      /* Firstly, port default */

      if (sport == 0) {
         sport = 6667 ;    /* Default IRC port is 6667 */
      }

      if ( userfile[0] == '\0' ) {

         strcpy (userfile, "userfile") ;

      }

      if ( com_char[0] == '\0' ) {

         strcpy (com_char, "!") ;

      }

      if ( username[0] == '\0' ) {

         printf("Error: User name not defined in configuration file.\n") ;
         exit_nicely (100) ;

      }

      if ( realname[0] == '\0' ) {

         strcpy (realname, "Ilona") ;

      }


      if ( server_name[0] == '\0' ) {
         printf("Error: Server name not defined in configuration file.\n") ;
         exit_nicely (100) ;

      }

      if ( nickname[0] == '\0' ) {
         printf("Error: Nickname not defined in configuration file.\n") ;
         exit_nicely (100) ;

      }

      if ( channel[0] == '\0' ) {
         printf("Error: Channel not defined in configuration file.\n") ;
         exit_nicely (100) ;

      }

      if ( html_log == 1 ) {

          if ( html_header[0] == '\0') {

           printf("Error: No HTML header file defined (HTML logging mode selected).\n") ;
           exit_nicely (100) ;

        }

      }

   } else {

      #ifdef AMIGA
      MUI_RequestA(0,0,0,"Error","Ok","Couldn't find configuration file.", 0) ;
      #else
      printf("Error: Couldn't find configuration file.\n") ;
      #endif

      exit_nicely (100) ;

   }

}

void grab_userfile ( void) {

   FILE *upnt ;
   char string[81] ;
   char *str ;
   char id[20], host[40], status[10], oper[10] ;
   int count ;

   no_users = -1 ;

   /* tmg296 soton.ac.uk GOD OPER */

   /* Attempt userfile loading */

   upnt = fopen (userfile, "r") ;
   
   if ( upnt != 0) {

      /* File found...load and parse */
      /* Only load one line for the moment */

      while ( fgets (string, (int) sizeof(string)-1, upnt) !=0 ) {

         id[0]='\0' ;   /* Terminate in case of blank line */

         sscanf (string, "%s %s %s %s", &id, &host, &status, &oper) ;

         if ( (id[0] != '#') && (id[0] != '\0' ) ) {

            no_users++ ;

            /* We can basically just copy this in verbatim */
            /* Won't bother checking stuff just yet */

            strcpy (user_id[no_users], id) ;
            strcpy (user_host[no_users], host) ;
            strcpy (user_status[no_users], status) ;
            strcpy (user_oper[no_users], oper) ;

            /* Now need to skip to after oper part */

            count = strlen (id) + strlen (host) + strlen (status) + strlen(oper) + 4 ;
            str = string ;
            str = str + count ;   /* Actually move our pointer to point here */

            strcpy (user_greet[no_users], str) ;   /* And copy the sentence */

         }

      }

      fclose (upnt) ;

   } else {

      #ifdef AMIGA
      MUI_RequestA(0,0,0,"Error","Ok","Couldn't find users file.", 0) ;
      #else
      printf("Error: Couldn't find users file.\n") ;
      #endif

      exit_nicely (100) ;

   }

}

void grab_comfile ( void) {

   FILE *comfile ;
   char string[200] ;
   char *str ;
   char c_name[20], c_lev[5], c_type[6] ;
   int count ;

   /* Routine only called if COMMAND_FILE is specified */
   /* Therefore, the file should exist.... */

   no_coms = -1 ;

   comfile = fopen (commandfile, "r") ;
   
   if ( comfile != 0) {

      /* File found...load and parse */
      /* Only load one line for the moment */

      while ( fgets (string, (int) sizeof(string)-1, comfile) !=0 ) {

         c_name[0]='\0' ;   /* Terminate in case of blank line */

         sscanf (string, "%s %s %s", &c_name, &c_lev, &c_type) ;

         if ( (c_name[0] != '#') && (c_name[0] != '\0' ) ) {

            no_coms++ ;

            /* We can basically just copy this in verbatim */
            /* Won't bother checking stuff just yet */

            strcpy (com_name[no_coms], c_name) ;
            strcpy (com_type[no_coms], c_type) ;

            /* Level needs to be converted actually... */

            if (stricmp (c_lev, "ANY") == 0) { com_level[no_coms] = 0 ; }
            if (stricmp (c_lev, "NONE") == 0 ) { com_level[no_coms] = 1 ; }
            if (stricmp (c_lev, "PRIV") == 0 ) { com_level[no_coms] = 2 ; }
            if (stricmp (c_lev, "GOD") == 0) { com_level[no_coms] = 3 ; }
            
            /* Now need to skip to after oper part */

            count = strlen (c_name) + strlen (c_lev) + strlen (c_type) + 3 ;
            str = string ;
            str = str + count ;   /* Actually move our pointer to point here */

            strcpy (com_text[no_coms], str) ;   /* And copy the sentence */
            count=strlen (com_text[no_coms]) ;
            com_text[no_coms][count-1] = '\0' ;

         }

      }

      fclose (comfile) ;

   } else {

      #ifdef AMIGA
      MUI_RequestA(0,0,0,"Error","Ok","Couldn't find user command file", 0) ;
      #else
      printf("Error: Couldn't find user command file!\n") ;
      #endif

      exit_nicely (100) ;

   }

}
void save_userfile (void) {

   FILE *userptr ;
   int count ;

   /* Over-write old userfile with current one */

   userptr = fopen (userfile, "w") ;

   if (userptr != 0) {

      /* Writing is okay */

      for (count=0; count<no_users+1; count++) {

         fprintf(userptr, "%s %s %s %s %s\n", user_id[count], user_host[count], user_status[count], user_oper[count], user_greet[count]) ;

      }

      fclose (userptr) ;

   } else {

      #ifdef AMIGA
      MUI_RequestA(0,0,0,"Error","Ok","Unable to write user file!",0 ) ;
      #else
      printf("Error: Unable to write user file!\n") ;
      #endif

      do_sys_log("Error: Unable to write user file.") ;
      exit_nicely(100) ;

   }

}





