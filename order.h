/*


*/
#ifndef _ORDER_H_   
#define _ORDER_H_
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h> 
#include <iostream>
#include "rf24_config.h"
#include "rf24hub_config.h"
#include "common.h"
#include "dataformat.h"


class Order {

private:

// Structure to handle the orderqueue
struct order_t {
        NODE_DATTYPE           node_id;   		// the destination node
        uint8_t                msg_id;          // ==> payload.msg_id
        uint8_t                msg_type;      	// ==> payload.msg_type
        uint8_t   	           msg_flags;     	// ==> payload.msg_flags
        ONR_DATTYPE	           orderno;   		// ==> payload.orderno
        uint32_t      	       data1;		    // The transportvalue for the Sensor 1 and Value 1
        uint32_t      	       data2;		    // The transportvalue for the Sensor 2 and Value 2
        uint32_t      	       data3;		    // The transportvalue for the Sensor 3 and Value 3
        uint32_t      	       data4;		    // The transportvalue for the Sensor 4 and Value 4
        uint32_t      	       data5;		    // The transportvalue for the Sensor 5 and Value 5
        uint32_t      	       data6;		    // The transportvalue for the Sensor 6 and Value 6
        uint64_t		       entrytime;       // Timestamp for creating of this record
        uint64_t	  	       last_send;		// Timestamp for last sending of this record
        order_t*               p_next;
};

order_t*  p_initial;
/**************************************************************
 * char buffer zur Ausgabe des timestrings ==> ts(tsbuf)
 *************************************************************/
char*       tsbuf;
char*       buf;
char*       buf1;
char*       buf2;
char*       buf3;
char*       buf4;
char*       buf5;
char*       buf6;
/**************************************************************
 * Bufferinterner Speicher für den verboselevel
 *************************************************************/
uint16_t    verboselevel;
/**************************************************************
 * Binärer Speicher der anzeigt ob mind. 1 Datensatz 
 * im Speicher ist.
 *************************************************************/
bool has_order;
/**************************************************************
 * Die aktuelle OrderNummer
 *************************************************************/
ONR_DATTYPE orderno;
/**************************************************************
 * fügt einen neuen record zum Buffer hinzu
 *************************************************************/
void newEntry(order_t* p_new);
/**************************************************************
 * löscht den übergebenen record aus dem Buffer
 *************************************************************/
bool delEntry(order_t* p_del);
/**************************************************************
 * findet den ersten record zum Node
 *************************************************************/
order_t* findNode(NODE_DATTYPE node_id);
    
public:
    
/**
 *  Setzt das Verboselevel
 */
void setVerbose(uint16_t _verboselevel);
/**
 * true wenn min ein Eintrag vorhanden sonst false
 */
bool hasEntry(void);
/**
 * true wenn die übergebene orderno einen record im Speicher hat
 */
bool isOrderNo(ONR_DATTYPE orderno);
/**
 * löscht den record im Buffer mit der übergebenen orderno
 */
bool delByOrderNo(ONR_DATTYPE orderno);
/**
 * löscht den record im Buffer mit der übergebenen node_id
 */
bool delByNode(NODE_DATTYPE node_id);
/**
 * fügt einen neuen record zum Buffer hinzu
 */
void addOrder(NODE_DATTYPE node_id, uint8_t msg_type, uint32_t data, uint64_t entrytime);
/**
 * Füllt das data Feld an der Position pos 2..6 => data2..data6
 */
void modifyOrder(NODE_DATTYPE node_id, uint8_t pos, uint32_t data);
/**
 * Setzt das msg_flag für die übergebene node_id
 */
void modifyOrderFlags(NODE_DATTYPE node_id, uint8_t msg_flags);
/**
 *  Setzt die entrytime auf den aktuellen Zeitpunkt für Orders zu diesem Node.
 *  Anwendung: Wird trotz Quittung ein Nachrichtentyp weiter empfangen,
 *  ist davon auszugehen das die Quittung nicht angekommen ist.
 *  Beispiel: HB (->in) HBSTOP (->out)
 */ 
void adjustEntryTime(NODE_DATTYPE node_id, uint64_t newEntrytime);
/**
 * füllt den Payload mit den Daten für die nächste Sendung
 */
bool getOrderForTransmission(payload_t* payload, uint64_t mytime); 
/**
 * Druckt alle records im Buffer in den out_socket
 * out_socket ist dabei ein gültiger socket file descriptor
 * entweder aus accept für einen socket oder mittels
 * fileno(stdout) für den stdout
 * Der zweite Parameter bestimmt das Format,
 * true => HTML Format; false => Textformat
 */
void printBuffer(int out_socket, bool htmlFormat);

Order(void);

};

#endif // _ORDER_H_
