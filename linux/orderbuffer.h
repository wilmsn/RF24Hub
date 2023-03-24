/**
 * @file orderbuffer.h
 * @brief Der Orderbuffer ist ein Objekt zur Speicherung von Einzelanforderungen.
 * 
 * Kommt eine Anforderung im Hub an, wird diese zunächst im Orderbuffer gespeichert.
 * Besteht Kontakt zu einem Node (bei ankommendem Heartbeat) werden alle offenen Orderbuffereinträge für diesen Node
 * zu einer Orderstruktur aggregiert. In einer Order können 6 Orderbuffereinträge zum Node übertragen werden.
 * 
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
 * Bufferinterner Speicher für den verboseLevel
 *************************************************************/
uint16_t    verboseLevel;
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
 * Setzt den Verboselevel. Mögliche Level sind in den Makros VERBOSE* in config.h definiert.
 * @param verboseLevel Der aktuelle Verboselevel
 */
void setVerbose(uint16_t _verboseLevel);

/**
 * @note Ruft den nächsten Record für einen Node ab.
 * Initialer Aufruf mit p_last = NULL.
 * Beim nächsten Aufruf wird der Rückgabewert des
 * letzten Aufrufs bei p_last übergeben
 * @param node_id Die Node_ID
 * @param p_last Beim ersten Aufruf NULL, sonst den Rückgabewert des letzten Aufrufs.
 * @param p_data Pointer auf ein data Feld. Dieses Feld ist nach dem Aufruf gefüllt.
 * @return Ein gesetzter Pointer wenn ein Datensatz gefunden wurde, sonst NULL.
 */
void* findOrder4Node(NODE_DATTYPE node_id, void* p_last, uint32_t* p_data);

/**
 * Fügt einen neuen record ein
 * @param millis Der aktuelle Zeitstempel mit mymillis() erzeugt
 * @param node_id Die Node_ID
 * @param channel Der Channel
 * @param data Die zu übermittelnden Daten als Transportwert 
 */
void addOrderBuffer(uint64_t millis, NODE_DATTYPE node_id, uint8_t channel, uint32_t data);

/**
 * Löscht den record für die übergebe Kombinaltion von node_id und channel
 * @param node_id Die Node_ID
 * @param channel Der Channel
 * @return "true" wenn erfolgreich, sonst "false"
 */
bool delByNodeChannel(NODE_DATTYPE node_id, uint8_t channel);

/**
 * Löscht alle records für die übergebene node_id
 * @param node_id Die Node_ID
 * @return "true" wenn erfolgreich, sonst "false"
 */
bool delByNode(NODE_DATTYPE node_id);

/**
 * Gibt es mindestens einen record für den übergebenen node_id?
 * @param node_id Die Node_ID
 * @return "true" wenn Records vorhanden, sonst "false"
 */
bool nodeHasEntry(NODE_DATTYPE node_id);

/**
 * Druckt alle records im Buffer in den out_socket.
 * out_socket ist dabei ein gültiger socket file descriptor
 * entweder aus accept für einen socket oder mittels
 * fileno(stdout) für den stdout
 * Der zweite Parameter bestimmt das Format,
 * true => HTML Format; false => Textformat
 * @param tn_socket Das aktuelle Socket zur Ausgabe
 * @param htmlformat "true" liefert die Ausgabe im HTML Format, "false" im Textformat
 */
void printBuffer(int out_socket, bool htmlFormat);

OrderBuffer(void);

};

#endif // _ORDERBUFFER_H_
