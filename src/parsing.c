/* Ilona - parsing.c */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "bot.h"
#include "config.h"
#include "logging.h"
#include "parsing.h"
#include "servers.h"
#include "main.h"
#include "users.h"

extern struct sys_config *sc;

char temp_line[100];
char temp_line2[100];

struct irctab irctable[] =
{

    {"PING", parse_ping },
    {"001", parse_001 },
    {"353", parse_353 },
    {"372", parse_372 },
    {"433", parse_433 },
    {"473", parse_473 },
    {"JOIN", parse_join },
    {"PRIVMSG", parse_privmsg },
    {"ERROR", parse_error },
    {NULL, NULL}

};

struct ilonatab ilonatable[] =
{

    {"OP", 2, ilona_op },
    {"HELP", 1, ilona_help },
    {"QUIT", 10, ilona_quit },
    {"RAW", 10, ilona_raw },
    {"SHOWCHANUSERS", 10, ilona_showchanusers},
    {"SHOWCOMMANDS", 1, ilona_showcommands},
    {NULL, 0, NULL}

};

/* Search for a given token */

char *GetToken(char *source, char *starttokens, char *endtokens)
{
    char *src, *tok=NULL;
    int done=0;

    src=source;
    while((*src) && (strchr(starttokens, *src)))
        src++;
    if(*src)
    {
        tok=src;
        while(!done)
        {
            if(*src==0)
                done=1;
            else if(strchr(endtokens, *src))
            {
                *src++='\0';
                done=1;
            }
            else
                src++;
        }
        source=src;
    }
    return(tok);
}

/* Return NUH */

void GetNUH (char *source)
{

    /*
    char nuh[50];

    */

}

struct User * find_user (struct Bot *b, char *lf)
{

    int i,j;
    char *where, *where2, *temp, *lf_save;

    lf_save=lf;

    for (i=1; i<=b->no_channels; i++)
    {

        for (j=1; j<= get_channel(b, i)->no_users; j++)
        {

            /* First, check to see if userID, i.e. tgreaves is present */

            temp = strstr ( lf, "@") ;
            *temp='\0';

            do_bot_log(b, "Comparsing %s with %s (UserID check)", lf, get_user (get_channel(b,i),j)->NUH);

            where = strstr ( get_user ( get_channel(b,i), j)->NUH, lf);

            /* Now check if the userhost, i.e. enterprise.net is present */

            *temp='@';    /* Restore */
            lf = temp+1;  /* Move lf to point at userhost */

            /* Temporaily hack stored userhost */

            temp = strstr ( get_user (get_channel(b,i),j)->NUH, "@");
            temp++;     /* So we are pointing at userhost part ONLY */

            do_bot_log(b, "Comparsing %s with %s (Userhost check)", lf, temp);
            where2 = strstr ( lf, temp);

#ifdef DEBUG_USERSEARCH
            do_bot_log (b, "Comparing with %s", get_user ( get_channel (b,i), j)->NUH);
#endif

            lf=lf_save;   /* Back again! */

            if ( (where != NULL) && (where2 != NULL) )
            {

                /* We found it here */
                return get_user (get_channel(b,i),j);

            }

        }

    }

    return NULL;  /* not found */

}

struct ChanUser * find_chanuser (struct Bot *bo, struct Channel *b, char *lf)
{

    int j;
    char *where, *where2, *temp, *lf_save;

    if (b->no_cusers == 0)
    {
        return NULL ;
    }

    lf_save=lf;

    for (j=1; j<= b->no_cusers; j++)
    {

        /* First, check to see if userID, i.e. tgreaves is present */

        temp = strstr ( lf, "@") ;
        *temp='\0';

        do_bot_log(bo, "Comparsing %s with %s (UserID check)", lf, get_chanuser (b,j)->NUH);
        where = strstr ( get_chanuser ( b, j)->NUH, lf);

        /* Now check if the userhost, i.e. enterprise.net is present */

        *temp='@';    /* Restore */
        lf = temp+1;  /* Move lf to point at userhost */

        /* Temporaily hack stored userhost */


        temp = strstr ( get_chanuser (b,j)->NUH, "@");
        temp++;     /* So we are pointing at userhost part ONLY */

        do_bot_log(bo, "Comparsing %s with %s (Userhost check)", lf, temp);
        where2 = strstr ( lf, temp);

#ifdef DEBUG_USERSEARCH
        do_bot_log (b, "Comparing with %s", get_chanuser ( b, j)->NUH);
#endif

        lf=lf_save;   /* Back again! */

        if ( (where != NULL) && (where2 != NULL) )
        {

            /* We found it here */
            return get_chanuser (b,j);

        }

    }

    return NULL;  /* not found */

}


/* PARSE_PING = handles server PINGs which are sent every so often */

void parse_ping (struct Bot *b, char *f)
{

    SendToServer (b, "PONG %s", f) ;

    do_bot_log (b, "Server PING received. Responding.");

}

/* 353 - Bot being informed of who is on a channel */
/* We simply send USERHOST messages to build up our picture of the channel */

void parse_353 (struct Bot *b, char *f)
{

    do_bot_log (b, "353 input is: %s", f);

}

/* 001 - Bot has been successfully connected to the IRC server */

void parse_001 (struct Bot *b, char *f)
{

    int i;

    /* This only sends the channel join commands. We'll obviously deal with */
    /* any errors when the server chucks them our way */

    b->error_connects=0;

    for (i=1; i<=b->no_channels; i++)
    {

        do_bot_log (b, "Attempting to join channel %s", get_channel (b,i)->channel);
        SendToServer (b, "JOIN %s", get_channel (b, i)->channel);

    }

}

/* 372 - The MOTD. Log this if we have been told to */

void parse_372 (struct Bot *b, char *f)
{

    if (b->log_motd == 1)
    {

        do_bot_log(b, f+ strlen (get_nick (b, b->current_nick))+2);

    }

}

/* 433 - Called if there is a nickname clash */

void parse_433 (struct Bot *b, char *f)
{

    do_bot_log (b, "Nickname %s is already in use.", get_nick (b, b->current_nick));

    b->current_nick--;

    if ( b->current_nick == 0)
    {

        /* We've tried all possible nicknames - abort */

        do_bot_log (b, "No nicknames left to try. Aborting this bot.");
        kill_socket (b);
        b->connected=0;

    }
    else
    {

        /* Try the next nickname then */

        do_bot_log (b, "Trying nickname %s", get_nick (b, b->current_nick));
        SendToServer(b, "NICK %s", get_nick (b, b->current_nick));

    }

}

/* 473 - Called if we can't join a channel (invite only) */

void parse_473 (struct Bot *b, char *f)
{

    sscanf (f, "%s %s", temp_line, temp_line2) ;

    do_bot_log (b, "Couldn't join %s - Channel is invite only.", temp_line2);

}

/* JOIN - Called when someone joins a channel we are on */

void parse_join (struct Bot *b, char *f)
{

    int found=0;
    int i;
    char *lf;
    char *f2;
    char *temp, *nik;
    struct ChanUser *us;
    struct User *use;
    struct Channel *chan;

    do_bot_log (b, "JOIN by %s, rest of line: %s", b->lastfrom, f);

    lf=b->lastfrom;
    f2=f;
    lf++;					/* Bypass colon */
    f2++;					/* Ditto */

    /* First check if it is *us* that has joined a channel */

    temp = strchr ( lf, '!' );
    *temp='\0';

    if ( strcasecmp ( lf, get_nick (b, b->current_nick) ) == 0)
    {

        do_bot_log (b, "Bot joined channel. Ignoring...") ;

    }
    else
    {

        /* It's someone else */

        nik=lf;         /* Remember nickname location */
        lf = temp+1;    /* Now lf points at userhost */

        /* Which channel ?? */

        for ( i=1; i<=b->no_channels; i++)
        {

            if ( strcasecmp ( get_channel (b, i)->channel, f2)==0)
            {

                found=1;
                chan = get_channel (b,i);

            }

        }

        if (found == 0)
        {

            do_bot_log (b, "Impossible error? Someone joined a channel I'm not on!");

        }
        else
        {

            /* Check if they should be auto-opped */

            use = find_user (b, lf);

            if (use && use->op && use->autoop)
            {

                SendToServer (b, "MODE %s +o %s", chan->channel, nik);

            }

            /* Found out if they are in the ChanUser list */
            /* NOTE: Do we actually need to check this? */

            us = find_chanuser (b, chan, lf);

            if ( us == NULL)
            {

                do_bot_log (b, "User is not already in the channel. Adding");
                insert_chanuser (chan);
                strcpy ( get_chanuser (chan, 1)->nick, nik);
                strcpy ( get_chanuser (chan, 1)->NUH, lf);

            }
            else
            {

                do_bot_log (b, "User is already on the channel.") ;

            }

        }

    }

}

/* PRIVMSG - Called upon receipt of a PRIVMSG line */
/* This could either be a channel message or a private message to us */

void parse_privmsg (struct Bot *b, char *f)
{

    char *where;
    char *lf, *lf_bak;
    char command[20];
    struct User *us;
    int i,found;

    lf_bak = lf = b->lastfrom;
    lf++;
    lf_bak++;

    sscanf (f, "%s ", temp_line);    /* Get nick/channel */

    if ( strcasecmp (temp_line, get_nick (b, b->current_nick)) == 0)
    {

        /* Wow, it's a message just for us */

#ifdef DEBUG_USERSEARCH
        do_bot_log (b, "Private message from %s - %s", lf, f + strlen (temp_line) + 2);
#endif

        /* Try and identify this user */
        /* Note that it could be in any channel user file */

        where=NULL;
        us=NULL;

#ifdef DEBUG_USERSEARCH
        do_bot_log (b, "Searching for that user in all user files.");
#endif

        /* Extract nick (i.e. go up to !) */

        lf = strstr ( lf, "!" ) +1;

#ifdef DEBUG_USERSEARCH
        do_bot_log (b, "Using %s", lf);
#endif

        us = find_user (b, lf) ;

        if ( us != NULL)
        {

#ifdef DEBUG_USERSEARCH
            do_bot_log (b, "Found the user - Nick stored: %s", us->nick);
#endif

            found=0;

            f = f + strlen (temp_line)+2;   /* First bypass nick and : */
            sscanf (f, "%s ", command);     /* Get command */

            /* We also want to store the nick of who just sent us this */
            /* lf_bak = beginning of nick!user@host */
            /* if     = one past the !   so... */

            lf--;
            *lf='\0';  /* Cunning, now if_bak is the nick */

            /* We will want to replace user->nick with this too */
            /* Now Ilona command subroutines have direct access */

            strcpy (us->nick, lf_bak);

            for(i=0; ilonatable[i].Name; i++)
            {

                if(strcasecmp(ilonatable[i].Name, command) == 0)
                {

                    found=1;

                    if ( us->level >= ilonatable[i].level)
                    {

                        /* User is privileged enough to use this command */

                        (ilonatable[i].Func)(b, us, f+ strlen (command)+1);

                    }
                    else
                    {

                        /* Insufficient level */

                        SendToServer (b, "NOTICE %s :Sorry, you are not a high enough level to use this command.", lf_bak);

                    }

                }

            }

            if (found==0)
            {

                do_bot_log(b, "Unknown command '%s' from %s", command, lf_bak);
                SendToServer (b, "NOTICE %s :Unknown command '%s'", lf_bak, command);

            }

        }
        else
        {

            /* User not in any of the user files! */

            do_bot_log(b, "Command received from unknown user-host %s", lf);
            lf--;
            *lf='\0';  /* Cunning, now if_bak is the nick */
            SendToServer (b, "NOTICE %s :Sorry, you are not in my user file.", lf_bak);

        }

    }
    else
    {

        /* Must be an in channel message - Ignore for now */
        /* At later date want to check for command character */

    }

}

/* ERROR - server error, so disconnect etc */

void parse_error (struct Bot *b, char *f)
{

    int attempts, ok, dns_fail;

    do_bot_log (b, "Server Error (possibly 'Too Many Connections')");
    kill_socket (b);
    b->connected = 0 ;

    /* We should try and connect again though */

    ok=1;

    prepare_socket (b);   /* Get a new one ready */

    b->error_connects++;

    if ( b->error_connects == b->max_connects)
    {

        /* Too many errors - go onto the next server */

        b->error_connects=0;
        b->current_server--;

        /* What if this was the last server? Wrap round! */

        if (b->current_server == 0)
        {
            b->current_server = no_IRCServers(b);
        }

    }

    /* Normal server connection stuff follows... */

    attempts=0;
    ok=0;
    dns_fail=0;

    while (ok!=1)
    {

        /* Now we've established which server, try connecting */

        attempts++;

        if ( attempts>b->max_connects )
        {

            /* Too many +connection+ retries - move onto the next server */
            b->current_server--;
            if (dns_fail!=1)
            {
                do_bot_log (b, "Maximum retires exceeded. Trying the next server.") ;
            }
            attempts=0;
            dns_fail=0;

            /* But what if this was the last server? Give us this time */

            if (b->current_server == 0)
            {

                do_bot_log(b,"No more servers left! Giving up for this bot.");
                return;

            }

        }
        else
        {

            if ( get_host_stuff (b, b->current_server))
            {

                /* Look-up was okay */
                b->port = get_IRCServerPort (b, b->current_server);

                if ( do_connect (b) == 1)
                {

                    /* Connection succeeded */
                    return;	/* So exit this */

                }
                else
                {

                    /* Connection failed */
                    /* Let loop again */

                }

            }
            else
            {

                /* Actual look-up failed. Don't try this server again */
                dns_fail=1;
                attempts=b->max_connects+1;
            }

        }

    }

}

/* ILONA COMMANDS */

/* ilona_help - help system for IRC users */

void ilona_help (struct Bot *b, struct User *us, char *f)
{

    char string[512];
    char *st;
    char command[20];
    FILE *help;

    do_bot_log (b, "HELP (%s) request received frmo %s", f, us->nick);

    help = fopen ("ilona.help", "r") ;

    if ( help != 0)
    {

        /* Only load one line for the moment */

        while ( fgets (string, (int) sizeof(string)-1, help) !=0 )
        {

            if ( string[0] == '/')
            {

                /* Beginning of a help description */
                st=string;
                st++;
                sscanf (st, "%s ", command) ;

                if ( strcasecmp ( command, f) == 0)
                {

                    /* User wanted help on this command */

                    while ( fgets (string, (int) sizeof(string)-1, help) != 0)
                    {

                        if ( string[0] == '+')
                        {
                            break;
                        }

                        SendToServer (b, "NOTICE %s :%s", us->nick, string);  /* Send it */

                    }

                    fclose(help);
                    return;   /* Help finished */

                }

            }

        }

        /* If we get here, no help was found */
        /* Just print up normal help stuff */

        fclose(help);

        SendToServer (b, "NOTICE %s :Hi, I'm %s, an Ilona %s bot.", us->nick, get_nick (b, b->current_nick), ILONA_VERSION);
        SendToServer (b, "NOTICE %s :For a list of commands type /msg %s showcommands", us->nick, get_nick (b, b->current_nick));

    }
    else
    {

        do_sys_log (sc, "Warning! Can't find ilona.help (cannot serve HELP requests)");
        SendToServer (b, "NOTICE %s :No help is avvailable at the moment.");

    }

}

/* ilona_op - asks for ops */

void ilona_op (struct Bot *b, struct User *us, char *f)
{

    char chan[100];	/* Channel name */
    char who[12];  	/* Who shoul we op ?? */
    int match=0;
    int i;

    struct Channel *chann;
    struct ChanUser *use;

    do_bot_log (b, "OP (%s) command received from %s", f, us->nick);

    if (b->no_channels > 1)
    {

        sscanf (f, "%s %s", chan, who);

    }
    else
    {

        sscanf (f, "%s ", who);
        strcpy (chan, get_channel (b,1)->channel) ;  /* Only one channel! */
    }

    /* First see if the channel exists */

    for (i=1; i<=b->no_channels; i++)
    {

        if ( strcasecmp (chan, get_channel (b, i)->channel) == 0)
        {
            match=i;
            chann = get_channel (b,i);
            break;

        }

    }

    if (match != 0)
    {

        /* Ok, the channel exists */
        /* Search to see if nickname is in channel currently */

        match=0;

        for (i=1; i<=chann->no_cusers; i++)
        {

            if ( strcasecmp (who, get_chanuser (chann, i)->nick)==0)
            {

                match=i;
                use = get_chanuser (chann,i);
                break;

            }

        }

        if (match != 0)
        {

            /* Ok, the user is in the channel */
            /* So do the op */

            SendToServer (b, "MODE %s +o %s", chann->channel, use->nick);

            if (chann->anonops == 0)
            {

                SendToServer (b, "NOTICE %s :You were opped by %s", use->nick, us->nick);

            }

        }
        else
        {

            SendToServer (b, "NOTICE %s :That user isn't in the channel.", us->nick);

        }

    }
    else
    {

        /* No such channel */

        SendToServer (b, "NOTICE %s :I'm not in that channel.", us->nick);

    }

}

/* ilona_showchanusers - shows users on a channel */
/* According to Ilona's file that is. */

void ilona_showchanusers (struct Bot *b, struct User *us, char *f)
{

    int found=0;
    int i;
    struct Channel *chan;

    do_bot_log (b, "SHOWCHANUSERS (%s) command received from %s", f, us->nick);

    if (b->no_channels == 1)
    {

        /* no need to parse argument. We are only on one channel */
        chan = get_channel ( b, 1);

    }
    else
    {

        /* Search for the specified channel */

        for (i=1; i<=b->no_channels; i++)
        {

            if ( strcasecmp ( get_channel (b,i)->channel, f) == 0)
            {

                found = 1;
                chan= get_channel (b,i);

            }

        }

        if ( found == 0 )
        {

            SendToServer (b, "NOTICE %s :I am not on channel %s", us->nick, f);
            return;

        }

    }

    /* Send user list */

    if (chan->no_cusers > 0)
    {

        SendToServer (b, "NOTICE %s :Current users on %s...", us->nick, chan->channel);

        for (i=1; i<=chan->no_cusers; i++)
        {

            SendToServer (b, "NOTICE %s :%s\t\t%s", us->nick, get_chanuser (chan, i)->nick, get_chanuser (chan,i)->NUH);

        }

    }
    else
    {

        SendToServer (b, "NOTICE %s :Apart from myself, there are no users on %s.", us->nick, chan->channel);

    }

}

/* ilona_showcommands - shows commands available to that user */

void ilona_showcommands (struct Bot *b, struct User *us, char *f)
{

    int i, j;
    int com_exist=0;
    char sctemp[512];
    int sctemp2;

    do_bot_log (b, "SHOWCOMMANDS command received from %s", us->nick);

    SendToServer (b, "NOTICE %s :The following commands are available to you:", us->nick);

    /* Scan through once for each level of user */

    for (i=1; i<=us->level; i++)
    {

        sctemp[0]='0';
        sprintf (sctemp, "Level %d: ", i);
        com_exist=0;

        for (j=0; ilonatable[j].Name; j++)
        {

            if ( i == ilonatable[j].level )
            {

                strcat (sctemp, ilonatable[j].Name) ;
                strcat (sctemp, ", ");                /* Nice comma */
                com_exist=1;

            }

        }

        /* If commands available, send the line */

        if (com_exist==1)
        {

            /* Note we will not want the final comma! */

            sctemp2 = strlen (sctemp);

            sctemp [sctemp2-2] = '\0';

            do_bot_log (b, "%s", sctemp);
            SendToServer (b, "NOTICE %s :%s", us->nick, sctemp);

        }

    }

    SendToServer (b, "Use /msg %s HELP <Command> for further information.", us->nick, get_nick (b, b->no_nicks));

}

/* ilona_quit - quits one Ilona bot */
/* (the shutdown command will do a total one) */

void ilona_quit (struct Bot *b, struct User *us, char *f)
{

    do_bot_log (b, "Quitting this bot, as ordered by %s", us->nick);
    SendToServer (b, "QUIT :Remote shutdown.");

    kill_socket(b);
    b->connected=0;

}

/* ilona_raw - allows a raw line to be sent to the server */

void ilona_raw (struct Bot *b, struct User *us, char *f)
{

    do_bot_log (b, "RAW (%s) command received from %s", f, us->nick);
    SendToServer (b, f);

}

/* Main IRC command parsing routine */
/* This deals with all data from the IRC server */

void parse_irc (struct Bot *b)
{

    char *buf;
    char command[100];
    int found=0;
    int i;

    buf = b->buffer;

    if (buf[0] != ':')
    {

        /* Direct server command */

        sscanf(buf, "%s ", command);

    }
    else
    {

        /* "Proper" IRC command */
        /* Copy who message is from to bot structure */

        sscanf (buf, "%s %s", b->lastfrom, command);
        buf=buf+strlen (b->lastfrom)+1;

    }

    for(i=0; irctable[i].Name; i++)
    {

        if(strcasecmp(irctable[i].Name, command) == 0)
        {

            found=1;

            (irctable[i].Func)(b, buf + strlen (command)+1);
            break;
        }

    }

#ifdef REPORT_UNKNOWNIRC

    if (found==0)
    {

        do_bot_log(b, "Unrecognised IRC command line: %s", buf);

    }

#endif

}
