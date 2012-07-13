// These are the logging routines.
// Based on original Ilona code, but global variable usage removed.

// Miscellaneous functions.

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "config.h"
#include "logging.h"

char *DateToString (unsigned long date) {

   static char datebuffer[10] ;
   struct tm *btime ;

//   btime = localtime ( (long *)&date) ;
//   strftime (datebuffer, 10, "%d-%b-%y", btime) ;
   return (datebuffer) ;
}

char *TimeToString (unsigned long date) {

   static char timebuffer[9] ;
   struct tm *btime ;

//   btime = localtime ((long *)&date) ;
//   strftime (timebuffer, 9, "%H:%M:%S", btime) ;
   return (timebuffer) ;
}

// Deal with system logging.

void do_sys_log ( sys_config sc, char *format, ... ) {

   char newbuf [512] ;
   va_list msg ;
   FILE *file ;
   long date ;

   va_start (msg, format) ;
   vsprintf(newbuf, format, msg) ;
   va_end (msg) ;

   if ( sc.systemlog[0] != '\0' ) {

      if ( file = fopen (sc.systemlog, "a" ) ) {

         date = time (NULL) ;

        // fprintf (file, "[%s %s] %s\n",  DateToString(date), TimeToString(date), newbuf) ;

         fclose (file) ;

      }

   }

}


