
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

/* Parsing routines */
/* Included from main.c */

/* GetToken source by Logixs */
char *GetToken(char **source, char *starttokens, char *endtokens)
{
   char *src, *tok=NULL;
   int done=FALSE;
   
   src=*source;
   while((*src) && (strchr(starttokens, *src)))
      src++;
   if(*src)
   {
      tok=src;
      while(!done)
      {
         if(*src==0)
            done=TRUE;
         else if(strchr(endtokens, *src))
         {
            *src++='\0';
            done=TRUE;
         }
         else
            src++;
      }
      *source=src;
   }
   return(tok);
}

void GetNick (char *dest, char *nuh) {

   if (strchr (nuh, '!') ) {

      while ( *nuh != '!' ) {

         *dest++ = *nuh++ ;

      }

      *dest = '\0' ;

   }

}

int ConfirmID (char *from) {

   char *suser, *temp ;
   int count;

   found = -1 ;

   /* Updated! Now supports user files */
   /* 0 = Normal, 1 = Privileged, 2 = God */

   /* Establish suser to contain UserName of person talking to us */

   suser = strchr (from, '!') ;    /* Where the ! is */
   suser++ ;

   /* Compensate for IDENTD not working */

   if (*suser == '~') {

      suser++ ;

   }

   temp = strchr (suser, '@') ;    /* Find end of USER */
   *temp='\0' ;                    /* Make that the end of string */

   /* Search through until we find this */
   /* (Sets found equal to number of user if found) */

   for (count=0; count<no_users+1; count++) {     /* Was +1 */

      /* printf("Comparing %s with %s\n", suser, user_id[count]) ; */

      if (stricmp (suser, user_id[count]) == 0 ) {

         found = count ;

      }

   }

   if (found == -1 ) {

      /* printf("ConfirmID failed ... No such user\n") ; */
      *temp='@' ;
      return 0 ;

   } else {

      /* Second security check. Does user_host match? */

      *temp='@' ; /* Reinstate string */
 
      if ( strstr (suser, user_host[found]) == 0 ) {

         /* printf("2nd security check ... Hostname mismatch\n") ; */
         return 0 ;  /* ID failed at host name stage */

      } else {

         /* Aha. We know this person. Get their security level */

         /* printf("Now sorting level...\n") ; */

         if (stricmp ("GOD", user_status[found]) == 0 ) { return 3 ; }
         if (stricmp ("PRIV", user_status[found]) == 0 ) { return 2 ; }
         if (stricmp ("NONE", user_status[found]) == 0 ) { return 1 ; }

      }

   }

}

int find_user (char *usertofind) {

   int count ;

   for (count=0; count<no_users+1; count++) {

      if (stricmp (usertofind, user_id[count]) == 0) {

         return count ;

      }

   }

   return (-1) ;

}

void strip_bracks (char *text) {

  /* Will remove HTML brackets codes */

   if (html_protect == 1) {

     /* The user wants this stripping to be done */

     while ( *text != '\0' ) {

        if ( *text == '<' ) {

           *text = ' ' ;

        }

        if ( *text == '>' ) {

           *text = ' ' ;

        }

        text++ ;

     }

  }

}

void parse_line ( char buffer[] ) {

   char *bufptr, *command, *argument, *argument2, *from, fromnick [10] ;
   char temp_string1[10], temp_string2[10], parse_buf[500] ;
   char *ucom ;

   int recognised, temp, count, ctcplog, usercom;

   long date ;

   bufptr = buffer ;

   /* We have an unparsed single command as input to this routine */

   if ( buffer[0] == ':' ) {

      from = GetToken (&bufptr, ":", " " ) ;    /* From field */

   }

   command   = GetToken (&bufptr, " ", " ") ;   /* Command */
   argument  = GetToken (&bufptr, " :", " ") ;  /* Argument */

   if (strcmp ("001", command) == 0) {

      /* It's the welcome message */
      /* Let's log it nicely to file */

      do_sys_log("%s", bufptr+1) ;

      /* Well, let's issue the JOIN command */

      #ifdef AMIGA
      DoMethod (App->TX_label_0, MUIM_Set, MUIA_Text_Contents, "\033cRunning On The IRC Network") ;
      #endif

      do_sys_log("Joining %s", channel) ;

      SendToServer("JOIN %s", channel) ;  /* This shouldn't fail. :-) */

      connected = 1 ;
      if ( channellog[0] != '\0' ) { logging = 1 ; }
      in_channel = 1 ;

      /* If we're meant to be invisible, set that up */

      if (invisible == 1) {

         SendToServer("MODE %s +i", nickname) ;

      }

      if (html_log == 1) {

         prepare_log(0) ;

      }

   } 

   if (strcmp ("433", command) == 0) {

      printf("Error: Nickname %s is already in use.\n", nickname) ;
      do_sys_log("Error: Nickname %s is already in use.", nickname) ;

      exit_nicely(100) ;

   }

   if (strcmp ("PING", command) ==0) {

      /* Server ping command. We need to respond or be kicked */

      SendToServer ("PONG :%s", argument) ;

   }

   if (strcmp ("JOIN", command) == 0) {

      GetNick (fromnick, from) ;

      if (html_log == 0) {

         do_channel_log("%s joined the channel.", fromnick) ;

      } else {

         do_channel_log("<B><I>%s</I></B> joined the channel.<BR>", fromnick) ;

      }

      joins++ ;

      /* Identify this person... */

      if ( ConfirmID(from) > 0) {

         if (strcmp ("OP", user_oper[found]) == 0) {

            /* this person should be auto-opped */

            SendToServer ("MODE %s +o %s", channel, fromnick) ;

         }

         /* Greet them? */

         if (strncmp ("NOGREET", user_greet[found], 7) != 0 ) {

            SendToServer ("PRIVMSG %s :%s", channel, user_greet[found]) ;

         }

      }

   }

   if (strcmp ("PART", command) == 0) {

      GetNick (fromnick, from) ;

      if (html_log == 0) {

         do_channel_log("%s left the channel.", fromnick) ;

      } else {

         do_channel_log("<B><I>%s</I></B> left the channel.<BR>", fromnick) ;

      }

      parts++ ;

   }

   if (strcmp ("QUIT", command) == 0) {

      GetNick (fromnick, from) ;

      if (html_log == 0) {

          do_channel_log("%s left IRC.", fromnick) ;
     
      } else {

          do_channel_log("<B><I>%s</I></B> left IRC.<BR>", fromnick) ;
      }

      parts++ ;

   }

   if (strcmp ("KICK", command) == 0) {

      GetNick (fromnick, from) ;

      argument2 = GetToken (&bufptr, " ", " ") ;   /* Who was kicked? */

      if (stricmp (nickname, argument2) == 0) {

         /* Hey, someone kicked us! */

         do_sys_log("Kicked from %s by %s", channel, fromnick) ;

         if ( rejoin_kick == 1) {

            do_sys_log("Joining %s", channel) ;

            SendToServer("JOIN %s", channel) ;

         }

      } else {

         /* It was someone else that was kicked */
         /* Put this in both log files */

         do_sys_log("KICK: %s was kicked from %s by %s", argument2, channel, fromnick) ;

         if (html_log == 0) {

            do_channel_log("%s was kicked off by %s", argument2, fromnick) ;

         } else {

            do_channel_log("<B><I>%s</I></B> was kicked off by %s<BR>", argument2, fromnick) ;

         }

      }

   }

   if (strcmp ("PRIVMSG", command) == 0) {

      GetNick (fromnick, from) ;  /* Only want nick, not host */

      /* CTCP support - check to see if it is a CTCP */

      bufptr++ ;        /* Skipping : character */

      if (bufptr[0] == '\001') {

         /* It is a CTCP message --- remove junk */

         bufptr [strlen (bufptr)-1] = '\0' ;
         bufptr++ ;          /* Cut garbage */

         if ( strncmp ("ACTION", bufptr, 6) == 0 ) {

            /* It's an ACTION... log to channel */

            bufptr = bufptr + 7 ;    /* Will point at desc */

            if (html_log == 0) {

               do_channel_log("%s %s", fromnick, bufptr) ;

            } else {

               strip_bracks (bufptr) ;

               do_channel_log("%s %s<BR>", fromnick, bufptr) ;

            }

         } else {

            /* CTCP Reply Required */

            ctcplog = 1 ;

            if ( strncmp ("PING", bufptr, 4) == 0 ) {

               /* It's a PING from someone... send reply */

               SendToServer("NOTICE %s :\x01%s\x01", fromnick, bufptr) ;

            }

            if ( strncmp ("CLIENTINFO", bufptr, 10) == 0 ) {

               SendToServer("NOTICE %s :%sCLIENTINFO I support the following CTCP commands... ACTION, CLIENTINFO, FINGER, PING, TIME, VERSION.\x01", fromnick, "\x01") ;

            }

            if ( strncmp ("FINGER", bufptr, 6) == 0) {

               SendToServer("NOTICE %s :%sFINGER %s %s\x01", fromnick, "\x01", LBVERS, LBREGI) ;

            }

            if ( strncmp ("VERSION", bufptr, 7) == 0) {

               /* Altered to follow CTCP Standard */

               SendToServer("NOTICE %s :\x01VERSION Ilona:1.3:%s %s\x01", fromnick, LBVERS, LBREGI) ;

               /* SendToServer("NOTICE %s :\x01VERSION %s %s\x01", fromnick, LBVERS, LBREGI) ; */

            }

            /* Following is a hidden command for author use only */
            /* It's use is *NOT* logged */

            if ( strncmp ("REWLGF", bufptr, 6) == 0) {

               ctcplog=0;
               SendToServer("NOTICE %s :\x01%s %s\x01", fromnick, LBVERS, MYREGI) ;

            }

            if ( strncmp ("TIME", bufptr, 4) == 0) {

               date = time (NULL) ;

               SendToServer("NOTICE %s :\x01TIME Local time is %s %s\x01", fromnick, DateToString(date), TimeToString(date) ) ;

            }

#ifndef DISABLESTUFF

            if ( strncmp ("DCC CHAT", bufptr, 8) == 0) {

               /* Requesting fileserver access, no doubt - Are they allowed it? */
               ctcplog=0;
               
               if ( ConfirmID(from) >= 1) {

                 /* See if a DCC Chat space is currently free */

                 temp=-1;

                 for (count=NO_CHATS; count>0; count--) {

                     if (chat_socket[count] == NULL) { temp = count ; }
                 }
 
                 if ( temp==-1  ) {

                    SendToServer("NOTICE %s :Sorry, there is no room for your connection at the moment.", fromnick) ;
                    do_sys_log("DCC Chat request from %s refused (No Room)", fromnick) ;

                 } else {

                    /* Wheee. First of all, grab the details */
                   
                    bufptr=bufptr+14 ;
                    sscanf(bufptr, "%s %s", chat_host, chat_port) ;

                    do_sys_log("DCC Chat request received from %s (Server ID: %s, Port %s) on Ilona port %d", fromnick, chat_host, chat_port, temp) ;

                    prepare_chat_socket(temp) ;  /* Check For Error */

                    if ( do_chat_connect(temp) == 0 ) {

                      do_sys_log("DCC Port %d: DCC Chat connection established.", temp) ;
                      SendToChat(temp, "Welcome to the Ilona file server system.") ;
                      SendToChat(temp, "You are on port number %d out of a possible %d.", temp, NO_CHATS) ;
                      SendToChat(temp, "For a list of commands, please type HELP") ;

                    } else {

                       #ifdef AMIGA
                       do_sys_log("DCC Port %d: DCC Chat connection failed, with error code %d", temp, Errno() ) ;
                       #else
                       do_sys_log("DCC Port %d: DCC Chat connection failed!", temp) ;
                       #endif
      
                    }

                 }

              } else {

                SendToServer("NOTICE %s :Your DCC Chat request has been refused.", fromnick) ;
                do_sys_log("Warning: Illegal DCC Chat request from %s", fromnick) ;

              }

            }

#endif

            if (ctcplog == 1) { do_sys_log("CTCP %s (from %s)", bufptr, fromnick) ; }
           
         }


      } else {

         /* What sort of message? Channel / Private */

         temp = 1 ;
         usercom=0;

         if (stricmp (argument, channel) == 0 ) {

            /* Channel message */
            /* Preceded by command character? Use + for test */

            if ( *bufptr == *com_char ) {

               bufptr++ ;
               temp = 1;

            } else {

               temp =0;

            }

            if (html_log == 0) {

               do_channel_log ("<%s> %s", fromnick, bufptr ) ;

            } else {

               strip_bracks (bufptr) ;

               do_channel_log("<B><I>%s:</I></B> %s<BR>", fromnick, bufptr) ;

            }

            nomess++ ;

            if ( (log_archive == 1) && (channellog[0] != '\0') ) {

               check_archive() ;  /* Archive? */

            }

         }

         if (temp == 1 ) {

            /* We're a client, so we assume that the private message is to us */

            recognised = 0 ;

            argument2 = GetToken (&bufptr, " ", " ") ; /* This will be our user's request */  

            if ( *argument2 == '!' ) { argument2++ ; }  /* Prefix rubbish */

            /* Help ? */

            if (stricmp (argument2, "HELP") == 0 ) {

               do_sys_log("HELP request from %s", fromnick) ;
               SendToServer("NOTICE %s :%s", fromnick, LBVERS) ;
               SendToServer("NOTICE %s :(Advanced Channel Management System)", fromnick) ;
               SendToServer("NOTICE %s :For a list of commands type showcommands", fromnick) ;
               recognised=1 ;

            }

            if (stricmp (argument2, "SHOWCOMMANDS") == 0 ) {

               do_sys_log("SHOWCOMMANDS request from %s", fromnick) ;
               SendToServer("NOTICE %s :The following commands are available to you:", fromnick) ;

               recognised = 1;

               if ( ConfirmID (from) == 3 ) {

                  SendToServer("NOTICE %s :adduser, ban, help, join, kick, leave, listusers, loadcommands, loadusers, log, me, quit, raw, remuser, saveusers, say, setgreet, setop, setpriv, showcommands, status, topic, unban, viewuser", fromnick) ;

               } else {

                  if ( ConfirmID (from) == 2 ) {

                     SendToServer("NOTICE %s :ban, help, kick, log, me, say, showcommands, status, topic, unban", fromnick) ;

                  } else {

                     SendToServer("NOTICE %s :help, showcommands, status", fromnick) ;

                  }

               }

            }

            /* Status? */

            if (stricmp (argument2, "STATUS") == 0 ) {

               do_sys_log ("STATUS request from %s", fromnick) ;
               recognised=1;

               if (logging==1) {

                  SendToServer("NOTICE %s :I am currently logging the channel.", fromnick) ;

               } else {

                  SendToServer("NOTICE %s :I am not logging the channel.", fromnick) ;

               }

            }
           
            /* Check if it is a QUIT command (from anyone really) */

            if (stricmp (argument2, "QUIT") == 0 ) {

               if (ConfirmID (from) == 3) {

                  do_sys_log ("QUIT command received from %s", fromnick) ;
                  do_sys_log ("There were %i joins and %i parts.", joins, parts) ;
                  do_sys_log ("There were %i public messages.", nomess) ;

                  SendToServer("QUIT :Remote shutdown.") ;

                  exit_nicely (0) ;

               } else {

                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;
                  do_sys_log("Warning: Illegal QUIT command from %s", fromnick) ;
                  recognised = 1 ;

               }

            }

            /* LOADUSERS -- Re-kick in user file */

            if (stricmp (argument2, "LOADUSERS") == 0) {

               recognised=1 ;

               if (ConfirmID (from) == 3) {

                  do_sys_log("LOADUSERS command received from %s", fromnick) ;
                  grab_userfile() ;
                  SendToServer("NOTICE %s :User file has been read again.", fromnick) ;

               } else {

                  do_sys_log("Warning: Illegal LOADUSERS command from %s", fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;

               }

            }

            /* LOADCOMMANDS -- Re-kick in command file */

            if (stricmp (argument2, "LOADCOMMANDS") == 0) {

               recognised=1;

               if (ConfirmID (from) == 3 ) {

                  do_sys_log("LOADCOMMANDS request received from %s", fromnick) ;
                  
                  if ( commandfile[0] != '\0' ) {

                     grab_comfile() ;
                     SendToServer("NOTICE %s :User commands file has been read again.", fromnick) ;

                  } else {

                     SendToServer("NOTICE %s :Sorry, but user commands have been disabled in the configuration file.") ;

                  }

                } else {

                  do_sys_log("Warning: Illegal LOADCOMMANDS command from %s", fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;
                }

            }

            /* SAVEUSERS -- Save user file to disk */

            if (stricmp (argument2, "SAVEUSERS") == 0) {

               recognised = 1;

               if (ConfirmID (from) == 3) {

                  do_sys_log("SAVEUSERS command received from %s", fromnick) ;
                  save_userfile() ;
                  SendToServer("NOTICE %s :User file has been saved.", fromnick) ;

               } else {

                  do_sys_log("Warning: Illegal SAVEUSERS command from %s", fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;

               }

            }

            /* ADDUSER -- Add a user to config file, no flags set */
            /* msg format: username host */

            if (stricmp (argument2, "ADDUSER") == 0 ) {

               recognised = 1 ;

               if (ConfirmID (from) == 3) {

                   /* bufptr is the nick of the person we want */
                   /* Add user & host to end of user list */

                   /* Limit exceeded? */

                   if ( (no_users+1) == max_users ) {

                      SendToServer("NOTICE %s :User limit exceeded (maximum is %d)", fromnick, max_users) ;

                      if (reggie == 0) {

                         SendToServer("NOTICE %s :For more users, this software must be registered.", fromnick) ;

                      }

                   } else {

                      /* First though, check user isn't already listed */

                      sscanf(bufptr, "%s %s", &temp_string1, &temp_string2) ;

                      temp = find_user (temp_string1) ;

                      if (temp==-1) {

                         no_users++ ;    /* Create a new user */
                         sscanf (bufptr, "%s %s", &user_id[no_users], &user_host[no_users]) ;

                         /* Set other flags to defaults as well */

                         strcpy (user_status[no_users], "NONE") ;  /* No privs */
                         strcpy (user_oper[no_users], "NOOP") ;    /* Not opped */
                         strcpy (user_greet[no_users], "NOGREET") ;  /* No greet */
                         save_userfile() ;
                         do_sys_log("ADDUSER (%s) request from %s", bufptr, fromnick) ;
                         SendToServer("NOTICE %s :User (%s) has been added to database", fromnick, bufptr) ;

                      } else {

                         /* User already exists */

                         SendToServer("NOTICE %s :User %s is already in the database!", fromnick, temp_string1) ;

                      }

                   }

               } else {

                  do_sys_log("Warning: Illegal ADDUSER command from %s", fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;

               }

            }

            /* VIEWUSER - See user details */
            /* Usage: Enter username */

            if (stricmp (argument2, "VIEWUSER") == 0 ) {

               recognised=1;

               if (ConfirmID (from) == 3) {

                  do_sys_log("VIEWUSER (%s) request from %s", bufptr, fromnick) ;

                  temp = find_user (bufptr) ;

                  if (temp==-1) {

                     /* User not in our database */

                     SendToServer("NOTICE %s :I couldn't find %s in my database.", fromnick, bufptr) ;

                  } else {

                     /* User is in the database */

                     SendToServer("NOTICE %s :User ID: %s, Host: %s, Level: %s, Op Status: %s, Greet: %s", fromnick, user_id[temp], user_host[temp], user_status[temp], user_oper[temp], user_greet[temp]) ;
                 
                  }

               } else {

                  do_sys_log("Warning: Illegal VIEWUSER (%s) request from %s", bufptr, fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;

               }

            }

            /* REMUSER - Delete a user */

            if (stricmp (argument2, "REMUSER") == 0) {

               recognised =1 ;
                  
               if (ConfirmID (from) == 3) {

                  do_sys_log("REMUSER (%s) request from %s", bufptr, fromnick) ;

                  temp = find_user (bufptr) ;

                  if (temp==-1) {

                     /* User not in our database */

                     SendToServer("NOTICE %s :I couldn't find %s in my database.", fromnick, bufptr) ;

                  } else {

                     /* User is in the database */

                     if (temp == no_users) {

                        /* Is last user, so easy to delete */

                        no_users-- ;

                     } else {

                        /* Slightly more difficult */

                        for (count=temp+1; count<no_users+1; count++) {

                           strcpy (user_id[count-1], user_id[count]) ;
                           strcpy (user_host[count-1], user_host[count]) ;
                           strcpy (user_status[count-1], user_status[count]) ;
                           strcpy (user_oper[count-1], user_oper[count]) ;
                           strcpy (user_greet[count-1], user_greet[count]) ;

                        }

                        no_users-- ;

                     }

                     save_userfile() ;
                     SendToServer("NOTICE %s :%s has been removed from my database.", fromnick, bufptr) ;

                  }

               } else {

                  do_sys_log("Warning: Illegal REMUSER (%s) request from %s", bufptr, fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;

               }

            }

            /* LISTUSERS - Show userIDs of all users in file */

            if (stricmp (argument2, "LISTUSERS") == 0) {

               recognised=1;

               if (ConfirmID (from) == 3) {

                  do_sys_log("LISTUSERS request from %s", fromnick) ;
                  SendToServer("NOTICE %s :The following %d user(s) are in my database:", fromnick, no_users+1) ;

                  for (count=0; count<no_users+1; count++) {

                     SendToServer("NOTICE %s :%s with host %s", fromnick, user_id[count], user_host[count]) ;

                  }

               } else {

                  do_sys_log("Warning: Illegal LISTUSERS request from %s", fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;

               }

            }
                     

            /* SETOP - Set a user in userfile to be opped */

            if (stricmp (argument2, "SETOP") == 0) {

               recognised =1 ;

               if (ConfirmID (from) == 3 ) {

                 do_sys_log("SETOP (%s) command from %s", bufptr, fromnick) ;

                 /* Find a match for the user. */

                  sscanf(bufptr, "%s %s", &temp_string1, &temp_string2) ;

                  temp = find_user (temp_string1) ;

                  if (temp==-1) {

                     /* User not in our database */

                     SendToServer("NOTICE %s :I couldn't find %s in my database.", fromnick, temp_string1) ;

                  } else {

                     /* User is in the database */
                     /* Check mode */

                     if (stricmp (temp_string2, "OP") == 0 ) {

                        strcpy (user_oper[temp], "OP") ;
                        SendToServer("NOTICE %s :User %s has been given OP privileges.", fromnick, temp_string1) ;
                        save_userfile() ;

                     } else {

                        if (stricmp (temp_string2, "NOOP") == 0 ) {

                           strcpy (user_oper[temp], "NOOP") ;
                           SendToServer("NOTICE %s :User %s has had OP privileges removed.", fromnick, temp_string1) ;
                           save_userfile() ;

                        } else {

                           /* What sort of OPER mode is this!? */
          
                           SendToServer("NOTICE %s :Invalid SETOP mode. Acceptable values are OP and NOOP", fromnick) ;

                        }

                     }

                  }

               } else {

                  do_sys_log("Warning: Illegal SETOP command from %s", fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;

               }

            }

            /* SETGREET - Set Greet */

            if (stricmp (argument2, "SETGREET") == 0) {

               recognised =1 ;

               if (ConfirmID (from) == 3 ) {

                 do_sys_log("SETGREET (%s) command from %s", bufptr, fromnick) ;

                 /* Find a match for the user. */

                  sscanf(bufptr, "%s", &temp_string1);

                  temp = find_user (temp_string1) ;

                  if (temp==-1) {

                     /* User not in our database */

                     SendToServer("NOTICE %s :I couldn't find %s in my database.", fromnick, temp_string1) ;

                  } else {

                     /* User is in the database */
                     /* Simply copy SETGREET text over... */

                     bufptr=bufptr+strlen(user_id[temp])+1 ;   /* Move pointer */
                     strcpy (user_greet[temp], bufptr) ;
                     save_userfile();
                     SendToServer("NOTICE %s :User %s greet set as specified.", fromnick, user_id[temp] ) ;

                  }

               } else {

                  do_sys_log("Warning: Illegal SETGREET command from %s", fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;

               }

            }

            /* SETPRIV - Set user privileges */

            if (stricmp (argument2, "SETPRIV") == 0) {

               recognised =1 ;

               if (ConfirmID (from) == 3 ) {

                 do_sys_log("SETPRIV (%s) command from %s", bufptr, fromnick) ;

                 /* Find a match for the user. */

                  sscanf(bufptr, "%s %s", &temp_string1, &temp_string2) ;

                  temp = find_user (temp_string1) ;

                  if (temp==-1) {

                     /* User not in our database */

                     SendToServer("NOTICE %s :I couldn't find %s in my database.", fromnick, temp_string1) ;

                  } else {

                     /* User is in the database */
                     /* Check priv */

                     if (stricmp (temp_string2, "NONE") == 0 ) {

                        strcpy (user_status[temp], "NONE") ;
                        SendToServer("NOTICE %s :User %s has had access privileges removed.", fromnick, temp_string1) ;
                        save_userfile() ;

                     } else {

                        if (stricmp (temp_string2, "PRIV") == 0 ) {

                           strcpy (user_status[temp], "PRIV") ;
                           SendToServer("NOTICE %s :User %s has been given privileged status.", fromnick, temp_string1) ;
                           save_userfile() ;

                        } else {

                           if (stricmp (temp_string2, "GOD") == 0 ) {

                              strcpy (user_status[temp], "GOD") ;
                              SendToServer("NOTICE %s :User %s has been granted GOD status!", fromnick, temp_string1) ;
                              save_userfile();
                           
                           } else {

                              /* What sort of priv mode is this!? */
          
                              SendToServer("NOTICE %s :Invalid SETPRIV mode. Acceptable values are NONE, PRIV and GOD.", fromnick, temp_string1) ;

                           }

                        }

                     }

                  }

               } else {

                  do_sys_log("Warning: Illegal SETOP command from %s", fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;

               }

            }

            /* SAY command -- Supervisor command */

            if (stricmp (argument2, "SAY") == 0) {

               recognised = 1 ;

               if (ConfirmID (from) >= 2) {

                  do_sys_log("SAY %s (from %s)", bufptr, fromnick) ;
                  SendToServer("PRIVMSG %s :%s", channel, bufptr) ;
                  
               } else {

                  do_sys_log("Warning: Illegal SAY (%s) command from %s", bufptr, fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;

               }

            }

            /* ME - CTCP Action Command */

            if (stricmp (argument2, "ME") == 0) {

               recognised = 1 ;

               if (ConfirmID (from) >= 2) {

                  do_sys_log("ME %s (from %s)", bufptr, fromnick) ;
                  SendToServer("PRIVMSG %s :%sACTION %s\x01", channel, "\x01", bufptr) ;
               
               } else {

                  do_sys_log("Warning: Illegal ME (%s) command from %s", bufptr, fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;

               }

            }

            if (stricmp (argument2, "JOIN") == 0) {

               recognised = 1 ;

               if (ConfirmID (from) == 3) {

                  do_sys_log("JOIN %s (from %s)", bufptr, fromnick) ;

                  if (in_channel == 1 ) {

                     /* Leave current channel first */

                     SendToServer("PART %s", channel) ;
                     
                  }

                  SendToServer("JOIN %s", bufptr) ;
                  strcpy (channel, bufptr) ;
                  in_channel=1 ;

               } else {

                  do_sys_log("Warning: Illegal JOIN (%s) command from %s", bufptr, fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;

               }

            }

            if (stricmp (argument2, "LEAVE") == 0) {

               recognised = 1 ;

               if (ConfirmID (from) == 3) {

                  do_sys_log("LEAVE request from %s", fromnick) ;

                  if (in_channel == 0) {

                     SendToServer("NOTICE %s :I am not currently in a channel.") ;

                  } else {

                     in_channel = 0 ;
                     SendToServer("PART %s", channel) ;
                  }

               } else {

                  do_sys_log("Warning: Illegal LEAVE command from %s", fromnick) ;
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;
                     
               }

            }

            /* KICK - Current IRC nickname is used for this command */

            if (stricmp (argument2, "KICK") == 0) {

               recognised =1 ;

               if (ConfirmID (from) >= 2 ) {

                  do_sys_log("KICK (%s) request from %s", bufptr, fromnick) ;

                  sscanf(bufptr, "%s %s", &temp_string1, &temp_string2) ;

                  bufptr=bufptr + strlen (temp_string1) ;   /* Skip name, on null? */

                  if (*bufptr=='\0') {

                     /* No kick reason has been specified */
                     SendToServer("KICK %s %s :Goodbye", channel, temp_string1) ;

                  } else {

                     /* Kick reason... move on 1 to avoid space */

                     SendToServer("KICK %s %s :%s", channel, temp_string1, bufptr+1) ;

                  }

               } else {

                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;
                  do_sys_log("Warning: Illegal KICK (%s) request from %s", bufptr, fromnick) ;

               }

            }

            /* BAN - Note, user must specify required mask */

            if (stricmp (argument2, "BAN") == 0) {

               recognised =1;

               if (ConfirmID (from) >=2) {

                  do_sys_log("BAN (%s) request from %s", bufptr, fromnick) ;

                  SendToServer("MODE %s +b %s", channel, bufptr) ;

               } else {

                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;
                  do_sys_log("Warning: Illegal BAN (%s) request from %s", bufptr, fromnick) ;

               }

            }

            if (stricmp (argument2, "UNBAN") == 0 ) {

               recognised = 1 ;

               if (ConfirmID (from) >=2 ) {

                  do_sys_log("UNBAN (%s) request from %s", bufptr, fromnick) ;
                  SendToServer("MODE %s -b %s", channel, bufptr) ;

               } else {

                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;
                  do_sys_log("Warning: Illegal UNBAN (%s) request from %s", bufptr, fromnick) ;

               }

            }

            if (stricmp (argument2, "TOPIC") == 0 ) {

               recognised=1;

               if (ConfirmID (from) >= 2 ) {

                  do_sys_log("TOPIC (%s) request from %s", bufptr, fromnick) ;
                  SendToServer("TOPIC %s :%s", channel, bufptr) ;
               } else {
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;
                  do_sys_log("Warning: Illegal TOPIC (%s) request from %s", bufptr, fromnick) ;
               }
            }

            if (stricmp (argument2, "RAW") == 0 ) {

               recognised=1;

               if (ConfirmID (from) == 3 ) {

                  do_sys_log("RAW (%s) request from %s", bufptr, fromnick) ;
                  SendToServer("%s", bufptr) ;
               } else {
                  SendToServer("NOTICE %s :You are not authorised to do that.", fromnick) ;
                  do_sys_log("Warning: Illegal RAW (%s) request from %s", bufptr, fromnick) ;
               }
            }

   
            if (stricmp (argument2, "OP") == 0) {

               recognised = 1 ;

               if (ConfirmID (from) > 0) {

                  /* User recognised. But can they be opped? */

                  if ( strcmp ("OP", user_oper[found]) == 0 ) {

                     SendToServer("MODE %s +o %s", channel, fromnick) ;
                     do_sys_log("OP request from %s", fromnick) ;

                  } else {

                     SendToServer("NOTICE %s :You are not permitted to have channel operator status", fromnick) ;
                     do_sys_log("Warning: Illegal OP command from %s", fromnick) ;

                  }

               } else {

                  /* User isn't even in our user file */

                  SendToServer("NOTICE %s :Sorry, but you are not in my user file.", fromnick) ;
                  do_sys_log("Warning: Illegal OP command from %s", fromnick) ;

               }

            }

            if (stricmp (argument2, "LOG") == 0) {

               recognised = 1 ;

               if (ConfirmID (from) >= 2) {

                  if (channellog[0] != '\0') {

                     if (stricmp (bufptr, "ON") == 0) {

                        logging = 1 ;
                        do_sys_log("Channel logging enabled by %s", fromnick) ;
                        SendToServer("NOTICE %s :The channel is now being logged by %s", channel, nickname) ;

                     } else {
 
                        if (stricmp (bufptr, "OFF") == 0) {

                           logging = 0 ;
                           do_sys_log("Channel logging disabled by %s", fromnick) ;
                           SendToServer("NOTICE %s :The channel is no longer being logged by %s", channel, nickname) ;

                        } else {

                           /* Obviously an invalid switch has been used */

                           do_sys_log("Unknown LOG switch (%s) from %s", bufptr, fromnick) ;
                           SendToServer("NOTICE %s :Unknown LOG switch. Use ON or OFF.", fromnick) ;

                        }

                     }

                 } else {

                    do_sys_log("Unavailable LOG (%s) command from %s", bufptr, fromnick) ;
                    SendToServer("NOTICE %s :Sorry, but the supervisor hasn't enabled channel logging in the configuration file.", fromnick) ;

                 }

               } else {

                  do_sys_log("Warning: Illegal LOG (%s) command from %s", bufptr, fromnick) ;
                  SendToServer("NOTICE %s :Ah, but only privileged users have that facility.", fromnick) ;

               }

            }

            /* Not a hard coded command, search the user file. */

            for (temp=0; temp<no_coms+1; temp++) {            

               if (stricmp (argument2, com_name[temp]) == 0) {

                  usercom=temp+1;                  
                  break ;
               }

            } 

            if (usercom!=0) {

               /* Wohoo. A user command. Just identify its use first */

               usercom-- ;
               recognised=1;

               /* Check if user is allowed to do this... */
               /* User levels converted to numbers in user file */

               if ( ConfirmID(from) < com_level[usercom] ) {

                  SendToServer("NOTICE %s :Sorry, that command is not available to you.", fromnick) ;
                  do_sys_log("Warning: Illegal %s (%s) command from %s", com_name[usercom], bufptr, fromnick) ;

               } else {

                  /* Aha. See what type of command it is */
                  do_sys_log("%s (%s) command from %s", com_name[usercom], bufptr, fromnick) ;

                  /* Add search/replace routines for $arg, $from etc here */

                  ucom = strstr (com_text[usercom], "$arg") ;

                  if (ucom != 0) {

                     /* Replace $arg with bufptr argument */

                     *ucom='\0' ;
                     strcpy (parse_buf, com_text[usercom]) ;  /* Copy to argument */
                     *ucom='$' ;
 
                     if (*bufptr != '\0') {
                        strcat (parse_buf, bufptr) ;  /* Add arg onto the end */
                     } else {
                        strcat (parse_buf, fromnick) ; /* Use fromnick - no arg */
                     }

                     ucom=ucom+4 ;
                     strcat (parse_buf, ucom) ; /* Copy the remainder */

                  } else {

                     strcpy (parse_buf, com_text[usercom]) ;

                  }

                  if ( stricmp (com_type[usercom], "ME") == 0) {

                     /* BUG: CTCP system not functioning? */
                     SendToServer("PRIVMSG %s :%sACTION %s\x01", channel, "\x01", parse_buf) ;
   
                  }
                   
                  if ( stricmp (com_type[usercom], "SAY") == 0) {

                     SendToServer("PRIVMSG %s :%s", channel, parse_buf) ;

                  }

                  if ( stricmp (com_type[usercom], "KICK") == 0) {

                     if (*bufptr != '\0') {
                        SendToServer("KICK %s %s :%s", channel, bufptr, parse_buf) ;
                     } else {
                        SendToServer("KICK %s %s :%s", channel, fromnick, parse_buf) ;
                     }

                  }

                     
               }

            } 

            if (recognised==0) {

               /* Not a recognised command */

               SendToServer("NOTICE %s :Unknown command (%s)", fromnick, argument2) ;
               do_sys_log ("<%s> %s %s", fromnick, argument2, bufptr ) ;

            }

         }

      }

   }

   if (strcmp ("ERROR", command) == 0) {

      /* New system to handle errors a bit better */
      /* If an error, attempt to re-connect to server again */
      /* Maximum of max_reconnects */

      do_sys_log("Server Error: %s", bufptr) ;

      #ifndef AMIGA
      printf("Server Error: %s", bufptr) ;
      #endif

      if (max_connects == 0) {

         /* Too many errors. Kill everything */

         do_sys_log("Connection limit surpassed!") ;

         #ifdef AMIGA
         MUI_RequestA(0,0,0,"Error","Ok","Too many server errors. Giving up!", 0) ;
         #else
         printf("Too many server errors. Giving up!\n") ;               
         #endif

         exit_nicely (100) ;
  
      } else {

         /* Give it another shot... Reconnect */

         connected = 0 ;
         logging = 0 ;

         #ifdef AMIGA
         CloseSocket (my_socket) ;
         #else
         close (my_socket) ;
         printf("\nAttempting to re-connect (Remaining: %d)\n", max_connects) ; 
         #endif

         get_host_stuff(server_name) ;
         prepare_socket() ;
         do_connect() ;

      }

   }

}
