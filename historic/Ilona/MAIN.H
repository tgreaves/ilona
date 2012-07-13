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

/* This is the header file for main.c */

#ifndef MAIN_H

#define MAIN_H

/* Library functions */

void open_libs (void) ;
void close_libs(void) ;

/* Startup-Exit Code */

void exit_nicely (int ec) ;
void SigInt (int sig) ;

void keyfile_handler (void) ;
void main(int argc, char * argv[] ) ;

#endif /* MAIN_H */
