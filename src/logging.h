/* Prototypes */

#ifndef LOGGING_H

#define LOGGING_H

char *DateToString (time_t *);
char *TimeToString (time_t *);
void do_sys_log (struct sys_config *, char *, ...) ;
void do_bot_log (struct Bot *, char *, ...);

#endif /* LOGGING_H */
