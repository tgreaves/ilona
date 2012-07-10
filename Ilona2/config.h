// Ilona II Configuration Structure

#ifndef CONFIG_H

#define CONFIG_H

// sys_config  ==   Configuration details which will apply to all bots that are running.
//                  (System logging filenames etc).

class sys_config {

    public:
    
    char systemlog [100] ;    // System log file filename.

    sys_config() {
        
        systemlog[0]='\0';
        
    }          // Constructor
    
    void set_systemlog (char *) ;

} ;

#endif // CONFIG_H
