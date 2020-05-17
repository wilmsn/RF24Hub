/*


*/
#ifndef _ORDERBUFFER_H_   
#define _ORDERBUFFER_H_
#include <stdio.h> 
#include <iostream>
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include "rf24_config.h"
#include "rf24hub_config.h"
#include "common.h"


class OrderBuffer {

// Structure to handle the orderqueue
    
    
private:

//char* buf;
struct orderbuffer_t {
        uint64_t		entrytime;
        NODE_DATTYPE   	node_id;
        uint8_t     	channel;
        uint32_t        utime;
        float        	value;
        orderbuffer_t*  p_next;
};
orderbuffer_t*     p_initial;
/**************************************************************
 * char buffer zur Ausgabe des timestrings ==> ts(buf)
 *************************************************************/
char*       buf;
/**************************************************************
 * Bufferinterner Speicher für den verboselevel
 *************************************************************/
uint16_t    verboselevel;
/**************************************************************
 * fügt einen neuen record zum Buffer hibzu
 *************************************************************/
void    newEntry(orderbuffer_t*);
/**************************************************************
 * löscht den übergebenen record aus dem Buffer
 *************************************************************/
bool    delEntry(orderbuffer_t*);

public:
    
/**************************************************************
 *  Setzt das Verboselevel
 *************************************************************/    
void setVerbose(uint16_t _verboselevel);
/**************************************************************
 *  Ruft den nächsten record für einen node ab.
 *  Initialer Aufruf mit p_last = NULL.
 *  Beim nächsten Aufruf wird der Rückgabewert des
 *  letzten Aufrufs bei p_last übergeben
 *************************************************************/    
void* findOrder4Node(NODE_DATTYPE node_id, void* p_last, uint8_t* channel, float* value);
/**************************************************************
 *  Fügt einen neuen record ein 
 *************************************************************/    
void addOrderBuffer(uint64_t millis, NODE_DATTYPE node_id, uint8_t channel, float value);
/**************************************************************
 *  Löscht den record für die übergebe Kombinaltion
 *  von node_id und channel
 *************************************************************/    
bool delByNodeChannel(NODE_DATTYPE node_id, uint8_t channel);
/**************************************************************
 *  Löscht alle records für die übergebene node_id
 *************************************************************/    
bool delByNode(NODE_DATTYPE node_id);
/**************************************************************
 *  Gibt es record für den übergebenen node_id 
 *  dann true sonst false
 *************************************************************/    
bool nodeHasEntry(NODE_DATTYPE node_id);
/*************************************************************
 *  Git den Inhalt des Nodebuffers über den übergebenen 
 *  telnet socket aus
 ************************************************************/
void printBuffer2tn(int new_tn_in_socket);
/**************************************************************
 *  Druckt alle records im Buffer in de tn_socket
 *************************************************************/
void htmlBuffer2tn(int new_tn_in_socket);
/**************************************************************
 *  Druckt den Inhalt des Buffers auf StdIO
 *************************************************************/
void printBuffer(void);    
OrderBuffer(void);

};

#endif // _ORDERBUFFER_H_
