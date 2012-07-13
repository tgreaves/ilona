/* Calculate some simple stats on a log */
/* Requires: grep, wc, sort, rm on system */ 

/* By Tristan Greaves */

#include <stdio.h>
#include <stdarg.h>

void do_grepstuff (char nick[], char dest[]) ;

void main (int argc, char * argv[]) {

  char buf[512] ;
  char pass[80] ;

  FILE *readit ;

  if (argc==3) {

     system("cd ~tmg296/temp/\n") ;

     readit = fopen (argv[1], "r") ;

     if ( readit != 0 ) {

        while ( fgets (buf, (int) sizeof(buf)-1, readit) !=0 ) {
      
          sscanf (buf, "%s", &pass) ;

          do_grepstuff (pass, argv[2]) ;

        }

        sprintf(buf, "sort -r %s -o %s ", argv[2], argv[2] ) ;
        system(buf) ;     

     } else {

        printf("Couldn't find read file.\n") ;

     }

  } else {
 
     printf("LogCount v1.0 - (c) Copyright 1997 Tristan Greaves.\n") ;
     printf("Part of the LogBot package.\n\n") ;
 
     printf("Outputs a sorted list of occurences to the destination file\n");
     printf("of the phrases listed in the read file.\n\n") ;

     printf("Usage: logcount <read file> <destination file>\n") ;

  }

}

void do_grepstuff ( char nick[], char dest[] ) {

   char newbuf [512] ;

   /* Grep all occurences */

   sprintf(newbuf, "grep %s ~tmg296/.WWW/cafelog.html >%s", nick, nick);
   system(newbuf) ;

   sprintf(newbuf, "wc --lines %s >>%s", nick, dest) ;
   system(newbuf) ;

   sprintf(newbuf, "rm %s -f", nick) ;
   system(newbuf) ;

}

