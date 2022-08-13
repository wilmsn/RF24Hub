/**
 * @file gateway.h
 * @brief In der Klasse Gateway werden alle dem System bekannten Gateways verwaltet
 * 
 * Zum Programmstart werden alle relevanten Informationen zu den Gateways aus der Datenbank geladen um dann im weiteren Programmablauf
 * hier verwaltet zu werden.
 * Wichtig: Wird ein neuer Gateway innerhalb der Datenbank angelegt, ist dieser zunächst für das Programm unbekannt.
 * Zur Synkronisation ist per telnet der Befehl "init" abzusetzen.
*/
#ifndef _GATEWAY_H_   
#define _GATEWAY_H_
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include "rf24_config.h"
#include "rf24hub_config.h"
#include "common.h"

using namespace std;

class Gateway {

    
private:

struct gateway_t {
        char*              gw_hostname;
        uint16_t           gw_no;
        time_t             last_contact;
        bool               isActive;
        gateway_t*         p_next;
    };

gateway_t*     p_initial;
/**************************************************************
 * char buffer zur generischen Verwendung
 *************************************************************/
char*          buf;
/**************************************************************
 * char buffer zur Ausgabe des timestrings ==> ts(tsbuf)
 *************************************************************/
char*          tsbuf;
/**************************************************************
 * Bufferinterner Speicher für den verboseLevel
 *************************************************************/
uint16_t       verboseLevel;
/**************************************************************
 * fügt einen neuen record zum Buffer hinzu
 *************************************************************/
bool           newEntry(gateway_t*);
/**************************************************************
 * löscht den übergebenen record aus dem Buffer
 *************************************************************/
bool           delEntry(gateway_t* p_del);

public:

/**
 * Gibt den Pointer auf einen Gateway zurück
 * Mit einem NULL Pointer wird die erste IP
 * und ein Pointer auf den nächsten Record
 * zurückgegeben.
 * Wenn ein NULL Pointer zurückgegeben wird, 
 * dann war es der letzte Record 
 * @param p_rec Ein Pointer auf die Gatewaystruktur
 * @param gw_hostname Ein Pointer zur Aufnahme des Hostnames des Gateways (Wird von der Prozedur gefüllt)
 * @param p_gw_no Ein Pointer zur Aufnahme der Gatewaynummer (Wird von der Prozedur gefüllt) 
 */
void* getGateway( void* p_rec, char* gw_hostname, uint16_t *p_gw_no );

/**
 * Setzt den Verboselevel. Mögliche Level sind in den Makros VERBOSE* in config.h definiert.
 * @param verboseLevel Der aktuelle Verboselevel
 */
void setVerbose(uint16_t _verboseLevel);

/**
 * Gibt an ob mind. ein Datensatz vorhanden ist
 * @return "true" wenn mind. 1 Gateway im System vorhanden ist, sonst "false"
 */
bool hasEntry(void);

/**
 *  Löscht den kompletten Inhalt und leert den Buffer
 */
void cleanup(void);

/**
 * Setzt ein Update auf den Zeitstempel des letzten Kontaktes zu diesem Gateway
 * @param gw_no Die Gatewaynummer 
 */
void gw_contact(uint16_t gw_no);

/**
 * Fügt einen neuen Gateway hinzu
 * @param gw_hostname Der Hostname mit Domaine des Gateways
 * @param gw_no Die Nummer des Gateways
 * @param isActive Statusflag ob dieser Gateway aktiv (=true) ist oder inaktiv (=false)
 */
void addGateway(char* gw_hostname, uint16_t gw_no, bool isActive );

/**
 * Entfernt einen Gateway
 * @param gw_no Die Nummer des Gateways
 */
void delGateway(uint16_t gw_no);

/**
 * Setzt einen Gateway in den übergebenen Status
 * @param gw_no Die Nummer des Gateways
 * @param isActive Statusflag ob dieser Gateway aktiv (=true) ist oder inaktiv (=false)
 */
void setGateway(uint16_t gw_no, bool isActive );

/**
 * Prüft ob ein Gateway aktiv (= true) ist.
 * @param gw_no Die Nummer des Gateways
 * @return "true" wenn der Gateway im System als aktiv gekennzeichnet ist, sonst "false"
 */
bool isGateway(uint16_t gw_no);

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
Gateway(void);

};

#endif // _GATEWAY_H_
