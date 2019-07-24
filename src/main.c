/* Ilona - main.c */
/* This deals with program initialization and the main bot loops */

#include <stdio.h>
#include <stdlib.h>            /* For exit() */
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "bot.h"
#include "config.h"
#include "main.h"
#include "logging.h"
#include "servers.h"
#include "parsing.h"

struct sys_config *sc;
struct Bots *thebots;
struct Bot *mybot;

int main (int argc, char *argv[])
{

    fd_set read_template;
    struct timeval wait;
    int nb, count, temp, ok, attempts, dns_fail;

    wait.tv_sec=40;
    wait.tv_usec=0;

    signal (SIGINT, SigInt);    /* Interrupts */

    titles();

    /* We need to get the sys_config and Bots structures ready */

    sc = (struct sys_config *)malloc(sizeof(struct sys_config));

    thebots = (struct Bots *)malloc(sizeof(struct Bots));
    thebots->bots = 0;

    /* Parse CLI arguments */
    /* And hence load in the configuration file */

    if (argc == 1)
    {

        load_config ("ilona.cfg");    /* Default filename */

    }
    else
    {

        if (argc == 2)
        {

            load_config (argv[1]);     /* Use whatever was passed on the CLI) */

        }
        else
        {

            printf("Usage: Ilona [Configuration Filename]\n");
            exit_nicely(100);

        }

    }

    do_sys_log(sc, "Ilona %s opened.", ILONA_VERSION);

    /* System log file check */

    /* Now go in a loop, attempting to connect each bot in turn */

    do_sys_log(sc, "Attempting bot connections.");
    temp=0;

    for (count=1; count<=no_bots(thebots); count++)
    {

        mybot = get_bot (thebots, count) ;
        mybot->current_server = no_IRCServers (mybot);   /* First on the list */

        do_bot_log(mybot, "Ilona %s opened.", ILONA_VERSION);

        ok=0;
        attempts=0;
        dns_fail=0;

        prepare_socket (mybot);

        while (ok != 1)
        {

            /* Need to try each server in turn */

            attempts++;

            if ( attempts>mybot->max_connects)
            {

                /* Too many re-tries - move onto the next server */
                mybot->current_server--;
                if (dns_fail!=1)
                {
                    do_bot_log (mybot,"Maximum retries exceeded. Trying the next server.");
                }
                attempts=0; /* Reset */
                dns_fail=0; /* Reset */

                /* What if this was the last server?? Give up */

                if (mybot->current_server == 0)
                {

                    do_bot_log (mybot,"No more servers left! Giving up for this bot.");
                    ok=1;

                }

            }
            else
            {

                if ( get_host_stuff (mybot, mybot->current_server))
                {

                    /* Look-up was okay */

                    mybot->port = get_IRCServerPort (mybot, mybot->current_server);

                    if ( do_connect (mybot) == 1)
                    {

                        /* Connection succeeded */
                        temp=1;
                        ok=1;

                    }
                    else
                    {

                        /* Connection failed for some reason */
                        /* While loop will re-execute, increasing attempts */

                    }

                }
                else
                {

                    /* Actual look-up failed - no point trying this server again! */
                    dns_fail=1;
                    attempts= mybot->max_connects+1;  /* Hence will move onto next server */

                }

            }

        }

    }

    do_sys_log(sc, "All connection attempts made.");

    /* If we haven't managed to connect any bots, we had might as well abort */

    if (temp == 0)
    {

        do_sys_log(sc, "Fatal Error: Couldn't connect any of the bots.");
        exit_nicely(100);

    }

    /* All bots now connected (hopefully!) so we now enter the main Ilona loop */

    do_sys_log(sc, "Ilona system now operational.");

    while (1)
    {

        /* Prepare socket selection waiting */
        /* This must be done each loop, as a bot may have "dropped out" */

        /* memset (&read_template, 0, sizeof(read_template)); */
        FD_ZERO(&read_template);
        ok=0;

        for (count=1; count<=no_bots(thebots); count++)
        {

            mybot = get_bot (thebots, count) ;

            if (mybot->connected)
            {

                FD_SET (mybot->sock, &read_template);
                ok=1;

            }

        }

        /* If no bots are left, we had best shut down */

        if (ok == 0)
        {

            do_sys_log(sc, "No bots remain active - Shutting down.");
            exit_nicely(0);

        }

        nb = select (FD_SETSIZE, &read_template, NULL, NULL, NULL);

        if (nb <=0)
        {

            /* do_sys_log(sc, "Socket time-out fault."); */

        }

        /* Process each bot to see if something was sent to it */

        for (count=1; count<=no_bots(thebots); count++)
        {

            mybot = get_bot (thebots, count);

            if (FD_ISSET (mybot->sock, &read_template) && mybot->connected==1)
            {

                grab_server_line (mybot);  /* Places in buffer */
                parse_irc (mybot);

            }

        }

    }

    exit_nicely(0);

}

void exit_nicely (int nice)
{

    int i;

    /* To exit safely, shutting down anything we need to. */

    /* Deal with system configuration structure (global) */

    do_sys_log (sc, "Ilona %s closed.\n", ILONA_VERSION);

    if (sc)
    {
        free(sc);
    }

    /* Also send exit message to the bot log-files */

    for (i=1; i<=no_bots(thebots); i++)
    {

        mybot=get_bot (thebots, i);
        do_bot_log (mybot, "Ilona %s closed.\n", ILONA_VERSION);

    }

    /* Kill the bots - no need for a safety check, as there will always be at least */
    /* one bot */

    kill_bots(thebots);

    if (thebots)
    {
        free (thebots);    /* Free Bot pointer structure */
    }

    exit(nice) ;  /* For now */

}

/* CTRL+C break signal trap */

void SigInt (int sig)
{

    /* Called when use requests a break, so we must exit nicely */

    do_sys_log(sc, "User break detected.") ;
    exit_nicely(0);

}

/* Simply print the intro text - CLI */

void titles (void)
{

    printf("Ilona %s - (c) Copyright 1996-2012 Tristan Greaves.\n\n", ILONA_VERSION);

}
