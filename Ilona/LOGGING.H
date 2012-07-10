
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

/* For logging.c */

/* Declare what we have in the file */

#ifndef LOGGING_H

#define LOGGING_H

/* Date-Time Functions */

char *DateToString (unsigned long date) ;
char *TimeToString (unsigned long date) ;

/* Log File Handling */

void prepare_log (int force) ;
void check_archive (void) ;

/* Actual Logging */

void do_sys_log ( char *format, ... ) ;
void do_channel_log ( char *format, ... ) ;

#endif /* LOGGING_H */
