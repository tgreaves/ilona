#ifndef USERS_H

#define USERS_H

/* Some quick prototypes */

void set_newuser (struct Channel *, char *);
void set_enduser (struct Channel *, char *);
void set_nick (struct Channel *, char *);
void set_NUH (struct Channel *, char *);
void set_level (struct Channel *, char *);
void set_op (struct Channel *, char *);
void set_autoop (struct Channel *, char *);

/* User file for a channel structure */
/* Pointed at by a bot->channel struct */

/* Channel user structure */

struct ChanUser
{

    char nick[12];							/* User's nickname */
    char NUH[30];							/* nick!user@host */
    struct ChanUser *next;				/* Pointer to the next one */

};

/* "Proper" user structure */
/* Associated with a channel */

struct User
{

    struct User *next;    /* Pointer to the next user */

    char nick[12];        /* Last nick we knew them by */
    char NUH[30];         /* nick!user@host - wildcarded?? */
    int level;				 /* The user's level */
    int op;               /* 1 = User may have ops, 0 = No */
    int autoop;				 /* 1 = Give ops on join, 0 = No */

};

/* uftab - The structure of the user-file command list */

struct uftab
{
    char  *Name;
    void (*Func)(struct Channel *, char *);
} ;

/* Prototypes */

void insert_user (struct Channel *);
struct User * get_user (struct Channel *, int);
void insert_chanuser (struct Channel *);
struct ChanUser * get_chanuser (struct Channel *, int);
void err_notinuser (struct Channel *, char *);
void load_users (struct Channel *);

#endif /* USERS_H */
