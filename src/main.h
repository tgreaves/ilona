/* main.h */

#ifndef MAIN_H

#define MAIN_H

#define ILONA_VERSION "v2.0"

/* #define PRINT_SYSLOG TRUE */
/* #define PARSE_CONFIG_ONLY TRUE */
#define REPORT_UNKNOWNIRC TRUE
/* #define DEBUG_USERSEARCH TRUE */

/* Prototypes */

void exit_nicely (int) ;      /* Shut everything down properly */
void SigInt (int);            /* Handle Amiga's BREAK signal */
void titles (void);           /* Print CLI titles */

#endif /* MAIN_H */
