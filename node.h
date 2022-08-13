/**
 * @file node.h
 * @brief Der Node wird als Objekt innerhalb der Klasse Node verwaltet.
 * 
 * Zum Programmstart werden alle relevanten Informationen zum Node aus der Datenbank geladen um dann im weiteren Programmablauf
 * hier verwaltet zu werden.
 * Wichtig: Wird ein Node innerhalb der Datenbank neu angelegt, ist dieser zunächst für das Programm unbekannt.
 * Zur Synkronisation ist per telnet der Befehl "init" abzusetzen.
*/
#ifndef _NODE_H_   
#define _NODE_H_
#include <stdint.h>
#include <unistd.h>
#include "rf24_config.h"
#include "rf24hub_config.h"
#include "common.h"

using namespace std;

class Node {

    
private:

//char* buf;
struct node_t {
        NODE_DATTYPE   	node_id;
        char            node_name[NODENAMESIZE];
        uint8_t         pa_level;
        uint32_t        pa_utime;
        float           u_batt;	
        uint8_t         hb_no;
        uint32_t        hb_utime;
        bool            is_mastered;
        node_t*         p_next;
    };
node_t*     p_initial;
/**************************************************************
 * char buffer zur generischen Verwendung
 *************************************************************/
char*       buf;
/**************************************************************
 * char buffer zur Ausgabe des timestrings ==> ts(tsbuf)
 *************************************************************/
char*       tsbuf;
/**************************************************************
 * Bufferinterner Speicher für den verboseLevel
 *************************************************************/
uint16_t    _verboseLevel;
/**************************************************************
 * fügt einen neuen record zum Buffer hibzu
 *************************************************************/
void        newEntry(node_t*);

public:

/**
 * Setzt den Verboselevel. Mögliche Level sind in den Makros VERBOSE* in config.h definiert.
 * @param verboseLevel Der aktuelle Verboselevel
 */    
void setVerbose(uint16_t verboseLevel);

/**
 * Gibt an ob ein Node als Datensatz vorhanden ist
 * @param node_id Die Node-ID
 * @return "true" wenn dieser Node bekannt ist, "false" wenn nicht.
 */
bool isValidNode(NODE_DATTYPE node_id);

/**
 *  Löscht den kompletten Inhalt und leert den Buffer
 */
void cleanup(void);

/**
 *  Fügt einen neuen Node hinzu
 * @param node_id Die Node-ID
 * @param node_name Der Name des Node 
 * @param u_batt Die aktuelle Batteriespannung
 * @param is_mastered "true" wenn dieser Hub diesen Node verwaltet, sonst "false"
 * @param PALevel Das gemessene PALevel
 * @param PAUtime Die Zeit (Unixtime) wann der PALevel ermittelt wurde
 */
void addNode(NODE_DATTYPE node_id, char* node_name, float u_batt, bool is_mastered, uint8_t PALevel, uint32_t PAUtime );

/**
 *  Gibt zur NodeID den Namen des Nodes zurück
 * @param node_id Die Node-ID
 * @return Der Name des Nodes als Zeiger auf einen String 
 */
char* getNodeName(NODE_DATTYPE node_id);

/**
 *  Prüft ob ein Heartbeat neu (=neue heartbeatno)  
 *  oder alt ist.
 * @param node_id Die Node-ID
 * @param heartbeatno Die vom Node übermittelte Heartbeat-Nummer
 * @param utime Die aktuelle Unix-times
 * @return "true" wenn es sich um einen neuen Heartbeat handelt, sonst "false" 
 */
bool isNewHB(NODE_DATTYPE node_id, uint8_t heartbeatno, uint32_t utime);

/**
 *  Prüft ob ein Node ein Node von diesem Hub gesteuert wird oder nicht 
 * @param node_id Die Node-ID
 * @return "true" wenn der Node durch diesen Hub verwaltet wird, sonst "false" 
 */
bool isMasteredNode(NODE_DATTYPE node_id);

/**
 *  Setzt einen Node in den Zustand "mastered" = von diesem Hub gesteuert; oder nicht 
 * @param node_id Die Node-ID
 * @param isMastered "true" wenn der Node durch diesen Hub verwaltet wird, sonst "false" 
 */
void setMasteredNode(NODE_DATTYPE node_id, bool isMastered);

/**
 * Setzt den letzten ermittelten PA Level für diesen Node
 * PA Level 0=unknown; 1=Min; 2=Low; 3=High; 4=Max
 * @param node_id Die Node-ID
 * @param pa_level Das ermittelte PA Level
 */
void setPaLevel(NODE_DATTYPE node_id, uint8_t pa_level);

/**
 * Liest den letzten ermittelten PA Level für diesen Node aus
 * PA Level 0=unknown; 1=Min; 2=Low; 3=High; 4=Max
 * @param node_id Die Node-ID
 * @return Das gespeicherte PA Level
 */
uint8_t getPaLevel(NODE_DATTYPE node_id);

/**
 * Setzt die letzte gemessene Spannung des Nodes
 * @param node_id Die Node-ID
 * @param u_batt Die aktuelle Batteriespannung
 */
void setVoltage(NODE_DATTYPE node_id, float u_batt);

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
void printBuffer(int tn_socket, bool htmlformat);

/**
 * Construktor des Buffers
 */
Node(void);

};

#endif // _NODE_H_
