
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

/* Header file for dcc.c */

#ifndef DCC_H

#define DCC_H

/* Preliminary Stuff */

int prepare_chat_socket(int the_sock) ;

/* Server Communications */

void SendToChat ( int the_sock, char *format, ...) ;
int grab_chat_line ( int the_sock, char output[] ) ;

/* Connections */

int do_chat_connect(int the_sock) ;

#endif /* DCC_H */
