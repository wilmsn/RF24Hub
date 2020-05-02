/*
 Buffer für Telnet Ausgaben, da Telnet als Thread gestartet wird und sich die Ausgaben sonst überschneiden können
 Ausgabe erfolgt im "main" Loop

*/
#ifndef _TN_TEXTBUFFER_H_   
#define _TN_TEXTBUFFER_H_
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h> 
#include <iostream>
#include "rf24hub_config.h"
#include "common.h"
#include "buffer.h"

extern uint16_t verboselevel;   

class Textbuffer : public Buffer{
    
private:

    // Structure to handle the orderqueue
    struct textbuffer_t {
        char       message[DEBUGSTRINGSIZE];		// Textbuffer
    };

public:
    
    bool hasMessage;
    void addMessage(char* newMessage);
    void printMessage(void);
    Textbuffer(void);


};

#endif // _TN_TEXTBUFFER_H_
