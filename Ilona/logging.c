
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

/* Logging Routines */
/* To be included by main.c */

char *DateToString (unsigned long date) {

   static char datebuffer[10] ;
   struct tm *btime ;

   btime = localtime ( (long *)&date) ;
   strftime (datebuffer, 10, "%d-%b-%y", btime) ;
   return (datebuffer) ;
}

char *TimeToString (unsigned long date) {

   static char timebuffer[9] ;
   struct tm *btime ;

   btime = localtime ((long *)&date) ;
   strftime (timebuffer, 9, "%H:%M:%S", btime) ;
   return (timebuffer) ;
}

void do_sys_log ( char *format, ... ) {

   char newbuf [512] ;

   va_list msg ;

   FILE *file ;

   long date ;

   va_start (msg, format) ;

   vsprintf(newbuf, format, msg) ;

   va_end (msg) ;

   if ( systemlog[0] != '\0' ) {

      if ( file = fopen (systemlog, "a" ) ) {

         date = time (NULL) ;

         fprintf (file, "[%s %s] %s\n",  DateToString(date), TimeToString(date), newbuf) ;

         fclose (file) ;

      }

   }

}

void prepare_log (int force) {

   /* See if we need to do channel log preparing */
   /* Called once channel joined */

   FILE *dest ;
   FILE *readfr ;

   char string[81] ;

   if (channellog[0] != '\0' ) {

      if ( ( dest = fopen (channellog, "r" ) ) && (force == 0 ) ) {

        /* File already exists */

        fclose (dest) ;

      } else {

        /* Now copy header across */

        fclose (dest) ; /* Close. We need to add to it */
        dest=NULL;

        if ( dest = fopen (channellog, "w") ) {

           if ( readfr = fopen (html_header, "r") ) {

              while ( fgets (string, (int) sizeof(string)-1, readfr) !=0 ) {

                 fputs (string, dest) ;

              }

              fclose(readfr) ;

           } else {

              do_sys_log("Warning: Couldn't open html header file (%s)", html_header) ;

           }

           fclose(dest) ;

        } else {

           do_sys_log("Warning: Couldn't open channel log file to copy html header.") ;

        }

     }

   }      
        
}

void check_archive (void ) {

   int check ;

   char *temp ;
 
   char string [200] ;

   char dname [100] ;

   long date ;

   FILE *dest, *readfr ;

   /* Determine :current: date */

   date = time (NULL) ;

   temp = DateToString (date) ;

   /* Comparisons ... */

   check = strncmp (temp, log_day,2) ; 

   if (check == 0) {

     /* It's the same day. Carry on as normal */

   } else {

     /* We need to archive this file */

     /* If statistics are wanted, we should add these */

     if (log_stats == 1) {

        if (html_log == 1) {

           do_channel_log("<BR><BR>There were %d joins, %d parts and %d messages.", joins, parts, nomess) ;

        } else {

           do_channel_log("\n\nThere were %d joins, %d parts and %d messages.", joins, parts, nomess) ;

        }

     joins = 0 ;
     parts = 0 ;
     nomess = 0 ;  /* Reset stats for next day */

     }

     strcpy (dname, channellog) ;

     strcat (dname, log_day) ;       /* Add day no. extension */ 

     dest = fopen (dname, "a") ;

     readfr = fopen (channellog, "r") ;

     while ( fgets (string, (int) sizeof(string)-1, readfr) !=0 ) {

        fputs (string, dest) ;

     }

     fclose(readfr) ;
     fclose(dest) ;

     do_sys_log("Current log archived (to %s)", dname) ;

     strncpy (log_day, temp, 2) ;  /* Update date */

     if (html_log == 1) {

        prepare_log(1) ;   /* Start current log afresh */

     } else {

        dest = fopen (channellog, "w") ; /* Refreshing... */
        fclose(dest) ;

     }

   }      

}     


void do_channel_log ( char *format, ... ) {

   if (logging==1) {

      char newbuf [512] ;

      va_list msg ;

      FILE *file ;

      long date ;

      date = time (NULL) ;

      if ( file = fopen (channellog, "a" ) ) {

         va_start (msg, format) ;

         vsprintf(newbuf, format, msg) ;

         va_end(msg) ;

         switch (log_stamp) {

            /* For date-time stamping option */

            case 1 : fprintf (file, "[%s] ", TimeToString(date) ) ;
                     break ;

            case 2 : fprintf (file, "[%s %s] ", DateToString(date), TimeToString(date) ) ;
                     break ;

            default : break ;

         }

         fprintf (file, "%s\n", newbuf) ;

         fclose (file) ;

      }

   }

}
