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
#include "dataformat.h"


class OrderBuffer {
    
private:

struct orderbuffer_t {
        uint64_t		entrytime;
        NODE_DATTYPE   	node_id;
        uint8_t     	channel;
        uint32_t        utime;
        uint32_t        data;
        orderbuffer_t*  p_next;
};
orderbuffer_t*     p_initial;
/**************************************************************
 * char buffer zur Ausgabe, generisch 
 *************************************************************/
char*       buf;
char*       buf1;
/**************************************************************
 * char buffer zur Ausgabe des timestrings ==> ts(tsbuf)
 *************************************************************/
char*       tsbuf;
/**************************************************************
 * Bufferinterner Speicher für den verboselevel
 *************************************************************/
uint16_t    verboselevel;
uint32_t    mykey;
/**************************************************************
 * fügt einen neuen record zum Buffer hibzu
 *************************************************************/
void    newEntry(orderbuffer_t*);
/**************************************************************
 * löscht den übergebenen record aus dem Buffer
 *************************************************************/
bool    delEntry(orderbuffer_t*);

public:
    
/**
 *  Setzt das Verboselevel
 */
void setVerbose(uint16_t _verboselevel);
/**
 *  Setzt den Key
 */
void setKey(uint32_t _key);
/**
 *  @note Ruft den nächsten Record für einen Node ab.
 *  Initialer Aufruf mit p_last = NULL.
 *  Beim nächsten Aufruf wird der Rückgabewert des
 *  letzten Aufrufs bei p_last übergeben
 *  @param node_id: Die Node_ID
 *  @param p_last: Beim ersten Aufruf NULL, sonst den Rückgabewert des letzten Aufrufs.
 *  @param p_data: Pointer auf ein data Feld. Dieses Feld ist nach dem Aufruf gefüllt.
 *  @return Ein gesetzter Pointer wenn ein Datensatz gefunden wurde, sonst NULL.
 */
void* findOrder4Node(NODE_DATTYPE node_id, void* p_last, uint32_t* p_data);
/**
 *  Fügt einen neuen record vom typ float ein 
 */
void addOrderBuffer(uint64_t millis, NODE_DATTYPE node_id, uint8_t channel, uint32_t data);
/**
 *  Löscht den record für die übergebe Kombinaltion
 *  von node_id und channel
 */
bool delByNodeChannel(NODE_DATTYPE node_id, uint8_t channel);
/**
 *  Löscht alle records für die übergebene node_id
 */
bool delByNode(NODE_DATTYPE node_id);
/**
 *  Gibt es record für den übergebenen node_id 
 *  dann true sonst false
 */
bool nodeHasEntry(NODE_DATTYPE node_id);
/**
 * Druckt alle records im Buffer in den out_socket.
 * out_socket ist dabei ein gültiger socket file descriptor
 * entweder aus accept für einen socket oder mittels
 * fileno(stdout) für den stdout
 * Der zweite Parameter bestimmt das Format,
 * true => HTML Format; false => Textformat
 */
void printBuffer(int out_socket, bool htmlFormat);

OrderBuffer(void);

};

#endif // _ORDERBUFFER_H_
