/* These are the logging routines. */
/* Based on original Ilona code, but global variable usage removed. */

/* Miscellaneous functions. */

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "bot.h"
#include "config.h"
#include "logging.h"
#include "main.h"

char *DateToString (time_t *dat) {

   static char datebuffer[10] ;
   struct tm *btime ;

   btime = localtime ( dat ) ;
   strftime (datebuffer, 10, "%d-%b-%y", btime) ;
   return (datebuffer) ;
}

char *TimeToString (time_t *dat) {

   static char timebuffer[9] ;
   struct tm *btime ;

   btime = localtime ( dat ) ;
   strftime (timebuffer, 9, "%H:%M:%S", btime) ;
   return (timebuffer) ;
}

/* Deal with system logging. */

void do_sys_log ( struct sys_config *sc, char *format, ... ) {

   char newbuf [512] ;
   va_list msg ;
   FILE *file ;
   time_t dat ;

   va_start (msg, format) ;
   vsprintf(newbuf, format, msg) ;
   va_end (msg) ;

   #ifdef PRINT_SYSLOG
	printf("[%s %s] %s\n", DateToString(&dat), TimeToString(&dat), newbuf);
   #endif

   if ( sc->systemlog[0] != '\0' ) {

      if ( file = fopen (sc->systemlog, "a" ) ) {

         dat = time (NULL) ;

         fprintf (file, "[%s %s] %s\n",  DateToString(&dat), TimeToString(&dat), newbuf) ;

         fclose (file) ;

      }

   }

}

void do_bot_log ( struct Bot *b, char *format, ... ) {

   char newbuf [512] ;
   va_list msg ;
   FILE *file ;
   time_t dat ;

   va_start (msg, format) ;
   vsprintf(newbuf, format, msg) ;
   va_end (msg) ;

	/* printf("[%s %s] %s\n", DateToString(&dat), TimeToString(&dat), newbuf); */

   if ( b->botlog[0] != '\0' ) {

      if ( file = fopen (b->botlog, "a" ) ) {

         dat = time (NULL) ;

         fprintf (file, "[%s %s] %s\n",  DateToString(&dat), TimeToString(&dat), newbuf) ;

         fclose (file) ;

      }

   }

}
