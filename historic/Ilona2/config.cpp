// config.cpp

#include <string.h>

#include "bot.h"
#include "config.h"
#include "main.h"

// sys_config functions

void sys_config::set_systemlog (char *f) {
    
    strcpy (systemlog, f);
    
}

// Deal with loading/parsing the configuration
//
// Supported are:
//
//   SERVER

   
