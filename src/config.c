/* config.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bot.h"
#include "config.h"
#include "main.h"
#include "parsing.h"
#include "users.h"

extern struct sys_config *sc;
extern struct Bots *thebots;
char temp_line[81];
int edit_bot;                 /* Set if we are currently defining a bot */
int edit_channel;					/* Set if we are currently defining a channel */
int line;                     /* Which line in config, to report errors */
int cfg_error;

struct configtab configtable[] =
{

    {"SYSTEM_LOG", set_systemlog },
    {"NEWBOT", set_newbot },
    {"ENDBOT", set_endbot },
    {"BOT_LOG", set_botlog },
    {"NICKNAME", set_nickname },
    {"USER_ID", set_userid },
    {"DESCRIPTION", set_description },
    {"SERVER", set_server },
    {"MAX_CONNECTS", set_maxconnects },
    {"LOG_MOTD", set_motd },
    {"NEWCHANNEL", set_newchannel },
    {"ENDCHANNEL", set_endchannel },
    {"CHANNEL", set_channel },
    {"USERFILE", set_userfile },
    {"CHANNEL_LOG", set_chanlog },
    {"ANONOPS", set_anonops },
    {NULL, NULL}

};

/* err_notinbot */
/* Short-cut to report attempt to define a command when not in a bot */

void err_notinbot (char *f)
{

    printf("Error parsing configuration (line %d) - Can't define %s outside a bot definition.\n", line, f);

}

/* SYSTEM_LOG */

void set_systemlog (char *f)
{

    sscanf (f, "%s ", sc->systemlog);

}

/* NEW_BOT - start setting up a new bot */

void set_newbot (char *f)
{

    if (edit_bot != 1)
    {

        insert_bot (thebots);
        edit_bot=1;

    }
    else
    {

        printf("Error parsing configuration (line %d) - Can't define a bot inside another bot!\n", line);
        cfg_error=1;
    }

}

/* END_BOT - stop setting up a new bot */

void set_endbot (char *f)
{

    struct Bot *mybot;

    if (edit_bot == 1)
    {

        /* Check that everything was defined */

        mybot = get_bot ( thebots, 1 ) ;

        if ( mybot->list == NULL )
        {

            printf("Error parsing configuration (line %d) - No IRC servers were defined for the bot.\n", line);
            cfg_error=1;

        }

        if ( mybot->nicks == NULL )
        {

            printf("Error parsing configuration (line %d) - No nicknames were defined for the bot.\n", line);
            cfg_error=1;

        }

        if ( mybot->channels == NULL )
        {

            printf("Error parsing configuration (line %d) - No channels were defined for the bot.\n", line);
            cfg_error=1;

        }

        if ( mybot->userid[0] == '\0')
        {

            printf("Error parsing configuration (line %d) - No UserID defined for bot!\n", line);
            cfg_error=1;

        }

        if ( mybot->description[0] == '\0')
        {

            printf("Error parsing configuration (line %d) - No Description defined for bot.\n", line);
            cfg_error=1;

        }

        if ( mybot->max_connects == 0)
        {

            printf("Error parsing configuration (line %d) - Max_Connects not specified for bot. Using default=3.\n", line);
            mybot->max_connects=3;

        }

        edit_bot = 0;

    }
    else
    {

        printf("Error parsing configuration (line %d) - Found END_BOT before NEW_BOT was defined.\n", line);

    }

}

/* SET_BOTLOG - set the bot filename */

void set_botlog (char *f)
{

    struct Bot *mybot;

    if (edit_bot == 1)
    {

        sscanf(f, "%s ", temp_line);
        mybot = get_bot ( thebots, 1) ;
        strcpy (mybot->botlog, temp_line);

    }
    else
    {

        err_notinbot ("Bot_Log");

    }

}

void set_userid (char *f)
{

    struct Bot *mybot;

    if (edit_bot == 1)
    {

        sscanf(f, "%s ", temp_line);
        mybot = get_bot (thebots, 1);
        strcpy (mybot->userid, temp_line);

    }
    else
    {

        err_notinbot ("User_ID");

    }

}

/* SET_DESCRIPTION - Set the WHOIS description of a bot */

void set_description (char *f)
{

    if (edit_bot == 1)
    {

        strcpy ( get_bot (thebots,1)->description, f);

    }
    else
    {

        err_notinbot ("Description");

    }

}

/* SET_SERVER - add an IRC server and its port to the bot */

void set_server (char *f)
{

    int port;
    char *temp;
    char *tail;

    if (edit_bot == 1)
    {

        sscanf (f, "%s %s ", temp_line, temp);    /* Split up */
        port = strtol (temp, &tail, 0);           /* Comvert port number */
        insert_IRCServer ( get_bot (thebots, 1), temp_line, port ) ;

    }
    else
    {

        err_notinbot ("Server");

    }

}

/* SET_NICKNAME - add a nickname to the bot */

void set_nickname (char *f)
{

    if (edit_bot == 1)
    {

        sscanf (f, "%s ", temp_line);
        insert_nick ( get_bot (thebots, 1), temp_line);

    }
    else
    {

        err_notinbot ("Nickname");

    }

}

void set_maxconnects (char *f)
{

    int no;
    char *tail;

    if (edit_bot == 1)
    {

        no = strtol (f, &tail, 0);     /* Comvert to a number */
        get_bot(thebots, 1)->max_connects = no;

    }
    else
    {

        err_notinbot ("Max_Connects");

    }

}

/* SET_MOTD - Whether the bot should log MOTD on its connection */
/* This is a switch - enabled if present, disabled if not */

void set_motd (char *f)
{

    if (edit_bot == 1)
    {

        get_bot(thebots,1)->log_motd=1;

    }
    else
    {

        err_notinbot ("Log_MOTD");

    }

}

/* SET_NEWCHANNEL - Prepare a new channel definition */

void set_newchannel (char *f)
{

    if (edit_bot == 1)
    {

        if (edit_channel ==0)
        {

            insert_channel ( get_bot(thebots, 1));
            edit_channel=1;

        }
        else
        {

            printf("Error parsing configuration (line %d) - Can't define a channel inside another one!\n", line);
            cfg_error=1;

        }

    }
    else
    {

        err_notinbot ("NewChannel");

    }

}

/* SET_ENDCHANNEL - Close channel definition, and check it */

void set_endchannel (char *f)
{

    struct Channel *mychan;

    if (edit_channel==1)
    {

        mychan= get_channel ( get_bot(thebots,1), 1);

        if ( mychan->channel[0]=='\0')
        {

            printf("Error parsing configuration file (line %d) - Channel name not defined!\n", line);
            cfg_error=1;

        }

        if ( mychan->userfile[0]=='\0')
        {

            printf("Error parsing configuration file (line %d) - Userfile not defined!\n", line);
            cfg_error=1;

        }

        if ( mychan->logfile[0]=='\0')
        {

            printf("Error parsing configuration file (line %d) - Channel_Log not defined!\n", line);
            cfg_error=1;

        }

        edit_channel=0;

    }
    else
    {

        printf("Error parsing configuration file (line %d) - Found EndChannel before NewChannel.\n", line);
        cfg_error=1;

    }

}

/* SET_CHANNEL - Set a channel's name */

void set_channel (char *f)
{

    if (edit_channel==1)
    {

        sscanf (f, "%s ", temp_line);
        strcpy ( get_channel(get_bot (thebots, 1), 1)->channel, temp_line);

    }
    else
    {

        printf("Error parsing configuration file (line %d) - Can't define Channel ouuside a NewChannel/EndChannel eefinition.\n", line);

    }

}

/* SET_USERFILE - Set file which contain users */

void set_userfile (char *f)
{

    if (edit_channel==1)
    {

        sscanf (f, "%s ", temp_line);
        strcpy ( get_channel(get_bot (thebots,1), 1)->userfile, temp_line);

    }
    else
    {

        printf("Error parsing configuration file (line %d) - Can't define Userfile outside a NewChannel/EndChannel definition.\n", line);

    }

}

/* SET_CHANLOG - Set channel log filename */

void set_chanlog (char *f)
{

    if (edit_channel==1)
    {

        sscanf (f, "%s ", temp_line);
        strcpy ( get_channel(get_bot (thebots,1), 1)->logfile, temp_line);

    }
    else
    {

        printf("Error parsing configuration file (line %d) - Can't define Channel_Log ouuside a NewChannel/EndChannel definition.\n", line);

    }

}

/* SET_ANONOPS - This is a flag */

void set_anonops (char *f)
{

    if (edit_channel==1)
    {

        get_channel(get_bot(thebots,1), 1)->anonops = 1 ;

    }
    else
    {

        printf("Error parsing configuration file (line %d) - Can't define AnonOps ouuside a NewChannel/EndChannel definition.\n", line);

    }

}

void load_config (char *filename)
{

    FILE *cfgfile;
    char string[81], bakstring[81];
    /* char *command; */
    char command[20];
    int i, j;
    int found=0;

    line=0;
    edit_bot = 0;
    edit_channel =0;
    cfg_error = 0;

    if ( (cfgfile = fopen (filename, "r")) != NULL )
    {

        /* Load it */

        printf("Parsing main configuration file...\n");

        while ( fgets (string, (int) sizeof(string)-1, cfgfile) != 0 )
        {

            printf("Line: %s", string);

            line++;
            strcpy (bakstring, string);
            sscanf (string, "%s ", command);

            found=0;

            for(i=0; configtable[i].Name; i++)
            {

                if(strcasecmp(configtable[i].Name, command) == 0)
                {

                    printf("Command: %s", command);
                    found=1;
                    (configtable[i].Func)(bakstring + strlen (command)+1);

                }
            }

            if (found == 0 && (strlen (command)!=0) && command[0]!='#' && command[0]!=';')
            {

                /* Report than an unknown configuraton command was used */
                printf("Error parsing configuration (line %d) - Unknown command %s\n", line, command);

            }

            strcpy(string, "");
            strcpy(command, "");

        }

        fclose (cfgfile) ;

        printf("Completed configuration file parsing.\n");

        if (cfg_error==1)
        {
            exit_nicely(100);
        }

#ifdef PARSE_CONFIG_ONLY
        exit_nicely(0);
#endif

        /* Now load in the user files as well */

        printf("Parsing channel user files...\n");

        for (i=1; i<=no_bots (thebots); i++)
        {

            for (j=1; j<=get_bot (thebots,i)->no_channels; j++)
            {

                load_users ( get_channel ( get_bot (thebots, i), j));

            }

        }

        printf("All done.\n");

    }
    else
    {

        /* Could not be found - this is a fatal error */
        /* But we can't do a syslog, so print it */

        printf("Fatal Error: Could not load configuration file %s\n", filename);
        exit_nicely(100);

    }

}
