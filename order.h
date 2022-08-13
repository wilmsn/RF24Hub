/**
 * @file order.h
 * @brief Die Klasse Order ist ein Objekt zur Speicherung von Bestellungen an einen Node.
 * 
 * Kommt eine Anforderung im Hub an, wird diese zunächst im Orderbuffer gespeichert.
 * Besteht Kontakt zu einem Node (bei ankommendem Heartbeat) werden alle offenen Orderbuffereinträge für diesen Node
 * zu einer Orderstruktur (in der Klasse Order) aggregiert. In einer Order können 6 Orderbuffereinträge zum Node übertragen werden.
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
 * Bufferinterner Speicher für den verboseLevel
 *************************************************************/
uint16_t    verboseLevel;
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
 * Setzt den Verboselevel. Mögliche Level sind in den Makros VERBOSE* in config.h definiert.
 * @param verboseLevel Der aktuelle Verboselevel
 */
void setVerbose(uint16_t _verboseLevel);

/**
 * Fragt die Orderstruktur ab ob mind. ein Eintrag vorhanden ist
 * @return "true" wenn mind. ein Eintrag vorhanden sonst "false"
 */
bool hasEntry(void);

/**
 * true wenn die übergebene orderno einen record im Speicher hat
 * @param orderno Die Ordernummer
 * @return "true" wenn die übergebene orderno einen record innerhalb der Speicherstruktur hat, sonst "false"
 */
bool isOrderNo(ONR_DATTYPE orderno);

/**
 * löscht den record im Buffer mit der übergebenen orderno
 * @param orderno Die Ordernummer
 * @return "true" wenn Datensätze gelöscht wurden, sonst "false"
 */
bool delByOrderNo(ONR_DATTYPE orderno);

/**
 * löscht den record im Buffer mit der übergebenen node_id
 * @param node_id Die Node ID
 * @return "true" wenn Datensätze gelöscht wurden, sonst "false"
 */
bool delByNode(NODE_DATTYPE node_id);

/**
 * fügt einen neuen record zum Buffer hinzu
 * @param node_id Die Node ID
 * @param msg_type Der Nachrichtentyp
 * @param data Ein als Transportwert verpackter Datensatz. Wird im Feld data1 gespeichert.
 * @param entrytime Die Eintrittszeit erzeugt aus mymillis()
 */
void addOrder(NODE_DATTYPE node_id, uint8_t msg_type, uint32_t data, uint64_t entrytime);

/**
 * Füllt das data Feld an der Position pos 2..6 => data2..data6
 * @param node_id Die Node ID
 * @param pos Die Position auf der das Datenpaket gespeichert wird (2 = data2 ... 6 = data6)
 * @param data Ein als Transportwert verpackter Datensatz. Wird im Feld data<pos> gespeichert.
 */
void modifyOrder(NODE_DATTYPE node_id, uint8_t pos, uint32_t data);

/**
 * Setzt das msg_flag für die übergebene node_id
 * @param node_id Die Node ID
 * @param msg_flag Das Nachrichtenflag
 */
void modifyOrderFlags(NODE_DATTYPE node_id, uint8_t msg_flags);

/**
 *  Setzt die entrytime auf den aktuellen Zeitpunkt für Orders zu diesem Node.
 *  Anwendung: Wird trotz Quittung ein Nachrichtentyp weiter empfangen,
 *  ist davon auszugehen das die Quittung nicht angekommen ist.
 *  Beispiel: HB (->in) HBSTOP (->out)
 * @param node_id Die Node ID
 * @param newEntrytime Die neue Eintrittszeit erzeugt aus mymillis()
 */ 
void adjustEntryTime(NODE_DATTYPE node_id, uint64_t newEntrytime);

/**
 * füllt den Payload mit den Daten für die nächste Sendung
 * @param payload Ein Zeiger auf den payload
 * @param mytime Die aktuelle Zeit erzeugt aus mymillis()
 * @return "true" wenn der payload gefüllt wurde, sonst "false"
 */
bool getOrderForTransmission(payload_t* payload, uint64_t mytime);

/**
 * Druckt alle records im Buffer in den out_socket
 * out_socket ist dabei ein gültiger socket file descriptor
 * entweder aus accept für einen socket oder mittels
 * fileno(stdout) für den stdout
 * Der zweite Parameter bestimmt das Format,
 * true => HTML Format; false => Textformat
 * @param tn_socket Das aktuelle Socket zur Ausgabe
 * @param htmlformat "true" liefert die Ausgabe im HTML Format, "false" im Textformat
 */
void printBuffer(int out_socket, bool htmlFormat);

Order(void);

};

#endif // _ORDER_H_
