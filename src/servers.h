/* Internet servers header file */

#ifndef SERVERS_H

#define SERVERS_H

int get_host_stuff (struct Bot *, int) ;    /* Prepare for connection */
void prepare_socket (struct Bot *) ;        /* Prepare a TCP/IP socket */
void kill_socket (struct Bot *) ;           /* Kill a TCP/IP socket */
int do_connect (struct Bot *) ;             /* Connect to an IRC server */
void SendToServer (struct Bot *, char *format, ...) ;     /* Send message to IRC server */
void grab_server_line (struct Bot *);      /* Read line from IRC Server */

#endif /* SERVERS_H */


