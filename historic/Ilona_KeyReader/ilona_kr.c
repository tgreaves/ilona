/* Key file reader for Ilona */
/* (c) Copyright 1997 Tristan Greaves */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main (void) {

   char Key_Details[10] ;
   char User_ID[10]; 
   char User_Name[50] ;
   char User_Surname[50] ;
   char *tail ;
   int i, leng, checkit ;

   char key_version[5] ;
   char checkstr[10] ;
   int mycheck = 0 ;

   FILE *keyfile ;

   checkit=0 ;

   printf("\nWelcome to the Ilona Keyfile reader, for Ilona v1.2\n") ;
   printf("---------------------------------------------------\n\n") ;

   printf("Reading in ilona.key...\n") ;

   keyfile = fopen ("ilona.key", "r") ;

   if (keyfile != 0) {

      fscanf(keyfile, "%s %s %s %s %s %s", Key_Details, key_version, User_ID, User_Name, User_Surname, checkstr) ;

      fclose (keyfile) ;

   } else {

      printf("Error: Couldn't open ilona.key for writing.\n") ;
      exit(100) ;

   }

   /* Need to convert checkstr to an integer */

   checkit = strtol (checkstr, &tail, 0) ;
   
   /* Confirm that this *IS* an Ilona Key */

   if ( strcmp ("ILKEY", Key_Details) != 0 ) {

      printf("Error: This isn't an Ilona key file (ILKEY missing)\n") ;
      exit (50) ;

   }

   if ( strcmp ("12", key_version) != 0 ) {

      printf("Error: Incompatable keyfile version (Found %s)\n", key_version) ;
      exit (50) ;
   }

   printf("Decoding...\n") ;

   leng = strlen (User_ID) ;

   for (i=0; i<leng; i++) {

      User_ID[i]=User_ID[i] - (4 * (i+1) ) ;    /* Reverse encryption */
      mycheck=mycheck + ( User_ID[i] + 4 ) ;

   }

   /* Right, now to do the same thing with the User_Name */

   leng = strlen (User_Name) ;

   for (i=0; i<leng; i++) {

      User_Name[i] = User_Name[i] - (6 * (i+1)) ;  /* Reverse... */
      mycheck=mycheck + ( User_Name[i] + 6 ) ;

   }

   /* And surname */

   leng = strlen (User_Surname) ;

   for (i=0; i<leng; i++) {

      User_Surname[i] = User_Surname[i] - (3 * (i+1)) ;  /* Reverse... */
      mycheck=mycheck + ( User_Surname[i] + 3 ) ;

   }

   if (mycheck != checkit) {

      printf("Checksum Error: Illegal Keyfile?  Hard coded is %d, but computed is %d\n", checkit, mycheck) ;
      exit (50) ;

   }

   printf("\nUser ID is: %s\n", User_ID) ;
   printf("User Name is: %s %s\n\n", User_Name, User_Surname) ;


}
