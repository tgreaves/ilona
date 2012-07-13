/* Ilona II Configuration Structure */

#ifndef CONFIG_H

#define CONFIG_H

/* Prototypes */

void err_notinbot (char *);
void set_systemlog (char *);
void load_config (char *);
void set_newbot (char *);
void set_endbot (char *);
void set_botlog (char *);
void set_nickname (char *);
void set_userid (char *);
void set_description (char *);
void set_server (char *);
void set_maxconnects (char *);
void set_motd (char *);
void set_newchannel (char *);
void set_endchannel (char *);
void set_channel (char *);
void set_userfile (char *);
void set_chanlog (char *);
void set_anonops (char *);

/* sys_config  ==   Configuration details which will apply to all bots that are running. */
/*                  (System logging filenames etc) */

struct sys_config {

    char systemlog [100] ;    /*  System log file filename. */

} ;

/* configtab - The structure of the commands and the function to be called */

struct configtab {
   char  *Name;
   void (*Func)(char *);
} ;

#endif /* CONFIG_H */
