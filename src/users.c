/* Users.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bot.h"
#include "users.h"
#include "config.h"
#include "logging.h"
#include "main.h"

extern struct sys_config *sc;
char temp_line[81];
int edit_user;                 /* Set if we are currently defining a user */
int line;                      /* Which line in config, to report errors */
int cfg_error;

struct uftab uftable[] =
{

    {"NEWUSER", set_newuser },
    {"ENDUSER", set_enduser },
    {"NICK", set_nick },
    {"NUH", set_NUH },
    {"LEVEL", set_level },
    {"OP", set_op },
    {"AUTOOP", set_autoop },
    {NULL, NULL}

};

/* Insert a new user onto the user list */

void insert_user (struct Channel *b)
{

    struct User *n;

    n = (struct User *)malloc(sizeof(struct User));

    n->nick[0]='\0';
    n->NUH[0]='\0';
    n->level=0;
    n->op=0;
    n->autoop=0;

    n->next = b->users;       /* Make this point to the next one */
    b->users = n;             /* Put this on the front of the list */
    b->no_users++;            /* How many servers... */

}

/* Return the numbered user */

struct User * get_user (struct Channel *b, int no)
{

    struct User *temp;
    int count=0;

    for (temp=b->users; temp; temp=temp->next)
    {

        count++;
        if (count == no)
        {
            break;
        }

    }

    return temp;

}

/* Insert a new user onto the channel user list */

void insert_chanuser (struct Channel *b)
{

    struct ChanUser *n;

    n = (struct ChanUser *)malloc(sizeof(struct ChanUser));

    n->nick[0]='\0';
    n->NUH[0]='\0';

    n->next = b->cusers;      /* Make this point to the next one */
    b->cusers = n;            /* Put this on the front of the list */
    b->no_cusers++;            /* How many servers... */

}

/* Return the numbered user */

struct ChanUser * get_chanuser (struct Channel *b, int no)
{

    struct ChanUser *temp;
    int count=0;

    for (temp=b->cusers; temp; temp=temp->next)
    {

        count++;
        if (count == no)
        {
            break;
        }

    }

    return temp;

}

/* Generic error message */

void err_notinuser (struct Channel *chan, char *f)
{

    printf("Error parsing %s (line %d) - Can't define %s outside a user definition.\n", chan->userfile, line, f);

}

/* SET_NEWUSER */

void set_newuser (struct Channel *chan, char *f)
{

    if (edit_user != 1)
    {

        insert_user (chan);
        edit_user=1;

    }
    else
    {

        printf("Error parsing %s (line %d) - Can't define a user inside another user!\n", chan->userfile, line);
        cfg_error=1;

    }

}

/* SET_ENDUSER */

void set_enduser (struct Channel *chan, char *f)
{

    struct User *myuser;

    if (edit_user==1)
    {

        myuser= get_user ( chan, 1);

        if ( myuser->nick[0]=='\0')
        {

            printf("Error parsing %s (line %d) - User nickname not defined!\n", chan->userfile, line);
            cfg_error=1;

        }

        if ( myuser->NUH[0]=='\0')
        {

            printf("Error parsing %s (line %d) - User's nick!user@host not defined!\n", chan->userfile, line);
            cfg_error=1;

        }

        if ( myuser->level==0)
        {

            printf("Error parsing %s (line %d) - User's level not defined!\n", chan->userfile, line);
            cfg_error=1;

        }

        edit_user=0;

    }
    else
    {

        printf("Error parsing %s (line %d) - Found EndUser before NewUser!.\n", chan->userfile, line);
        cfg_error=1;

    }

}

/* SET_NICK */

void set_nick (struct Channel *chan, char *f)
{

    if (edit_user == 1)
    {

        sscanf (f, "%s ", temp_line);
        strcpy ( get_user (chan, 1)->nick, temp_line);

    }
    else
    {

        err_notinuser (chan, "Nick");

    }

}

/* SET_NUH */

void set_NUH (struct Channel *chan, char *f)
{

    if (edit_user == 1)
    {

        sscanf (f, "%s ", temp_line);
        strcpy ( get_user (chan, 1)->NUH, temp_line);

    }
    else
    {

        err_notinuser (chan, "NUH");

    }

}

/* SET_LEVEL */

void set_level (struct Channel *chan, char *f)
{

    char *tail;

    if (edit_user == 1)
    {

        sscanf (f, "%s ", temp_line);
        get_user (chan, 1)->level = strtol (temp_line, &tail, 0);

        /* printf("Set to: %d\n", get_user (chan,1)->level); */

    }
    else
    {

        err_notinuser (chan, "Level");

    }

}

/* SET_OP */
/* This is a switch, and is optional */

void set_op (struct Channel *chan, char *f)
{

    if (edit_user == 1)
    {

        get_user (chan,1)->op = 1;

    }
    else
    {

        err_notinuser (chan, "Op");

    }

}

/* SET_AUTOOP - also a switch */

void set_autoop (struct Channel *chan, char *f)
{

    if (edit_user ==1)
    {

        get_user (chan,1)->autoop = 1;

    }
    else
    {

        err_notinuser (chan, "AutoOp");

    }

}

void load_users (struct Channel *chan)
{

    FILE *cfgfile;
    char string[81], bakstring[81];
    char command[20];
    int i;
    int found=0;

    line=0;
    edit_user = 0;
    cfg_error = 0;

    if ( (cfgfile = fopen (chan->userfile, "r")) != NULL )
    {

        /* Load it */

        while ( fgets (string, (int) sizeof(string)-1, cfgfile) != 0 )
        {

            line++;
            strcpy (bakstring, string);
            sscanf (string, "%s ", command);

            found=0;

            for(i=0; uftable[i].Name; i++)
            {

                if(strcasecmp(uftable[i].Name, command) == 0)
                {

                    found=1;
                    (uftable[i].Func)(chan, bakstring + strlen (command)+1);

                }
            }

            if (found == 0 && (strlen (command)!=0) && command[0]!='#' && command[0]!=';')
            {

                /* Report than an unknown configuraton command was used */

                printf("Error parsing %s (line %d) - Unknown command %s\n", chan->userfile, line, command);

            }

        }

        fclose (cfgfile) ;

        if (cfg_error==1)
        {
            exit_nicely(100);
        }

    }
    else
    {

        /* Could not be found - this is a fatal error */
        /* But we can't do a syslog, so print it */

        printf("Fatal Error: Could not load user file %s\n", chan->userfile);
        exit_nicely(100);

    }

}

