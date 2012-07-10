// bot.h
// Header file for an individual Ilona bot. Remember, several "Ilonas" can be controlled
// from a single instance of the program.

#ifndef BOT_H

#define BOT_H

// Prototypes

// Class definition

class IRCServer {                   // Details for a single IRC server.
    
    friend class Bot;
    
    private:
    
    IRCServer (char *, IRCServer *);
        
    char Server[100] ;              // Where server name goes.
    IRCServer *next;                // Pointer to the next one.
    
} ;    

class Bot {

    public:

    Bot() { list=0; sock=0; }               // Constructor

    //IRCServer *Bot::at_end();       // Find end of server list
    void insertserver (char *);     // Insert a server on the list.    
    void getserver (int, char *);   // Return the numbered server.

    void prepare_socket(void);      // Prepare a TCP/IP socket.

    private:
    
    IRCServer *list;                // Pointer to IRC Server list.
    int sock;                       // TCP/IP socket.
            
} ;     

#endif // BOT_H
