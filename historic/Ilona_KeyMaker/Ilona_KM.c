/* Key file maker for Ilona */
/* (c) Copyright 1997 Tristan Greaves */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main (void) {

   char User_ID[10]; 
   char User_Name[50] ;
   char User_Surname[50] ;
   int i, leng, checkit ;

   FILE *keyfile ;

   checkit=0 ;

   printf("\nWelcome to the Ilona Keyfile maker, for Ilona v1.2\n") ;
   printf("--------------------------------------------------\n\n") ;

   printf("Please enter User ID: ") ;
   scanf("%s", User_ID) ;

   printf("Please enter User Name: ") ;
   scanf("%s %s", User_Name, User_Surname) ;

   printf("\nBuilding....\n") ;

   /* Do a fiddle with User_ID so peeps can't just search for it */

   leng = strlen (User_ID) ;

   for (i=0; i<leng; i++) {

      checkit=checkit + ( User_ID[i] + 4 ) ;
      User_ID[i]=User_ID[i] + (4 * (i+1) ) ;    /* Evil cackle */

   }

   /* Right, now to do the same thing with the User_Name */

   leng = strlen (User_Name) ;

   for (i=0; i<leng; i++) {

      checkit=checkit + ( User_Name[i] + 6 ) ;
      User_Name[i]=User_Name[i] + (6 * (i+1) ) ;    /* Eviller cackle */

   }

   /* Same for Surname */

   leng = strlen (User_Surname) ;

   for (i=0; i<leng; i++) {

      checkit=checkit + ( User_Surname[i] + 3 ) ;
      User_Surname[i]=User_Surname[i] + (3 * (i+1) ) ;    /* Eviller cackle */

   }

   printf("Checksum is %d\n\n", checkit) ;

   printf("Writing...\n") ;

   keyfile = fopen ("ilona.key", "w") ;

   if (keyfile != 0) {

      fprintf(keyfile, "ILKEY %s %s %s %s %d", "12", User_ID, User_Name, User_Surname, checkit) ;

      fclose (keyfile) ;

      printf("All Done.\n\n") ;

   } else {

      printf("Error: Couldn't open ilona.key for writing.\n") ;

   }

}
