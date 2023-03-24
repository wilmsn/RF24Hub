/**
 * @file nodeclass.h
 * @brief Der Node wird als Objekt innerhalb der Klasse NodeClass verwaltet.
 * 
 * Zum Programmstart werden alle relevanten Informationen zum Node aus der Datenbank geladen um dann im weiteren Programmablauf
 * hier verwaltet zu werden.
 * Wichtig: Wird ein Node innerhalb der Datenbank neu angelegt, ist dieser zunächst für das Programm unbekannt.
 * Zur Synkronisation ist per telnet der Befehl "init" abzusetzen.
*/
#ifndef _NODECLASS_H_   
#define _NODECLASS_H_
#include <stdint.h>
#include <unistd.h>
#include "rf24_config.h"
#include "rf24hub_config.h"
#include "common.h"

using namespace std;

class NodeClass {

    
private:

//char* buf;
struct nodeClass_t {
        NODE_DATTYPE   	node_id;
        char            node_name[NODENAMESIZE];
        uint8_t         pa_level;
        uint8_t         rec_level;
        float           u_batt;
        float           lv_volt;
        bool            lv_flag;
        uint8_t         hb_no;
        uint32_t        hb_utime;
        bool            is_mastered;
        nodeClass_t*    p_next;
    };
nodeClass_t*     p_initial;
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
void        newEntry(nodeClass_t*);

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
 * @param is_mastered "true" wenn dieser Hub diesen Node verwaltet, sonst "false"
 * @param PALevel Die Sendestärke
 * @param RECLevel Der gemessene Empfangslevel
 * @param lv_volt Kritische Batteriespannung
 * @param lv_flag Low Voltage Flag (Wenn gesetzt ist die kritische Batteriespannung unterschritten)
 */
void addNode(NODE_DATTYPE node_id, char* node_name, bool is_mastered, uint8_t pa_level, uint8_t rec_level, float lv_volt, bool lv_flag );

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
 *  Prüft ob ein Heartbeat aktuell (=aktuelle heartbeatno) ist.
 * @param node_id Die Node-ID
 * @param heartbeatno Die vom Node übermittelte Heartbeat-Nummer
 * @return "true" wenn es sich um einen neuen Heartbeat handelt, sonst "false" 
 */
bool isCurHB(NODE_DATTYPE node_id, uint8_t heartbeatno);

/**
 *  Prüft ob ein Node ein Node von diesem Hub gesteuert wird oder nicht 
 * @param node_id Die Node-ID
 * @return "true" wenn der Node durch diesen Hub verwaltet wird, sonst "false" 
 */
//bool isMasteredNode(NODE_DATTYPE node_id);

/**
 *  Setzt einen Node in den Zustand "mastered" = von diesem Hub gesteuert; oder nicht 
 * @param node_id Die Node-ID
 * @param isMastered "true" wenn der Node durch diesen Hub verwaltet wird, sonst "false" 
 * @return "true" wenn der Node in der Datenbank vorhanden ist, sonst "false" 
 */
bool setMasteredNode(NODE_DATTYPE node_id, bool isMastered);

/**
 * Setzt den letzten ermittelten Empfangslevel für diesen Node
 * REc Level 9=unknown; 0=Min; 1=Low; 2=High; 3=Max
 * @param node_id Die Node-ID
 * @param rec_level Die ermittelte Empfangsfeldstärke
 */
void setRecLevel(NODE_DATTYPE node_id, uint8_t rec_level);

/**
 * Liest den letzten ermittelten Empfangslevel für diesen Node aus
 * Rec Level 9=unknown; 0=Min; 1=Low; 2=High; 3=Max
 * @param node_id Die Node-ID
 * @return Die ermittelte Empfangsfeldstärke
 */
uint8_t getRecLevel(NODE_DATTYPE node_id);

/**
 * Setzt die aktuelle Sendefeldstärke für diesen Node
 * PA Level 9=unknown; 0=Min; 1=Low; 2=High; 3=Max
 * @param node_id Die Node-ID
 * @param pa_level Die aktuelle Sendefeldstärke
 */
void setPaLevel(NODE_DATTYPE node_id, uint8_t pa_level);

/**
 * Liest die aktuelle Sendefeldstärke für diesen Node aus
 * PA Level 9=unknown; 0=Min; 1=Low; 2=High; 3=Max
 * @param node_id Die Node-ID
 * @return Die aktuelle Sendefeldstärke
 */
uint8_t getPaLevel(NODE_DATTYPE node_id);

/**
 * Setzt die letzte gemessene Spannung des Nodes
 * @param node_id Die Node-ID
 * @param u_batt Die aktuelle Batteriespannung
 */
void setVoltage(NODE_DATTYPE node_id, float u_batt);

/**
 * Setzt das Low Voltage Flag für diesen Node
 * @param node_id Die Node-ID
 * @param lv_flag Das aktuelle Low Voltage Flag
 * @return "true" if the Low Voltage Flag has changed; else "false"
 */
bool setLVFlag(NODE_DATTYPE node_id, bool lv_flag);

/**
 * Setzt das Low Voltage Flag für diesen Node
 * @param node_id Die Node-ID
 * @param lv_volt Das aktuelle Low Voltage Spannung
 * @return "true" if the Low Voltage Flag has changed; else "false"
 */
void setLVVolt(NODE_DATTYPE node_id, float lv_volt);

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
NodeClass(void);

};

#endif // _NODECLASS_H_
