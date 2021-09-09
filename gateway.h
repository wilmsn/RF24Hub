/*


*/
#ifndef _GATEWAY_H_   
#define _GATEWAY_H_
#include <stdint.h>
#include <unistd.h>
#include "rf24_config.h"
#include "rf24hub_config.h"
#include "common.h"

using namespace std;

class Gateway {

    
private:

struct gateway_t {
        char*              gw_name;
        uint16_t           gw_no;
        char*              gw_ip;
        unsigned long      last_contact;       
        bool               isActive;
        gateway_t*         p_next;
    };
gateway_t*     p_initial;
/**************************************************************
 * char buffer zur generischen Verwendung
 *************************************************************/
char*       buf;
/**************************************************************
 * char buffer zur Ausgabe des timestrings ==> ts(tsbuf)
 *************************************************************/
char*       tsbuf;
/**************************************************************
 * Bufferinterner Speicher für den verboselevel
 *************************************************************/
uint16_t    verboselevel;
/**************************************************************
 * fügt einen neuen record zum Buffer hinzu
 *************************************************************/
bool    newEntry(gateway_t*);
/**************************************************************
 * löscht den übergebenen record aus dem Buffer
 *************************************************************/
bool    delEntry(gateway_t* p_del);

public:

/**************************************************************
 * Gibt den Pointer auf eine GW_IP zurück
 * Mit einem NULL Pointer wird die erste IP
 * und ein Pointer auf den nächsten Record
 * zurückgegeben.
 * Wenn ein NULL Pointer zurückgegeben wird, 
 * dann war es der letzte Record 
 *************************************************************/
void* getGateway( void* p_rec, char* p_gw_ip, uint16_t *p_gw_no );
/**************************************************************
 *  Setzt das Verboselevel
 *************************************************************/
void setVerbose(uint16_t _verboselevel);
/**************************************************************
 *  Gibt an ob mind. ein Datensatz vorhanden ist
 *************************************************************/
bool hasEntry(void);
/**************************************************************
 *  Löscht den kompletten Inhalt und leert den Buffer
 *************************************************************/
void cleanup(void);
/**************************************************************
 * 
 *************************************************************/
void gw_contact(char* gw_ip, uint16_t gw_no);
/**************************************************************
 *  Fügt einen neuen Gateway hinzu
 *************************************************************/
void addGateway(char* gw_name, uint16_t gw_no, bool isActive );
/**************************************************************
 *  Entfernt einen Gateway
 *************************************************************/
void delGateway(uint16_t gw_no);
/**************************************************************
 *  Setzt einen Gateway in den übergebenen Status
 *************************************************************/
void setGateway(uint16_t gw_no, bool isActive );
/*************************************************************
 *  Prüft ob ein Gateway aktiv (= true) ist.
 ************************************************************/
bool isGateway(uint16_t gw_no, char* gw_ip);
/*************************************************************
 * Druckt alle records im Buffer in den out_socket
 * out_socket ist dabei ein gültiger socket file descriptor
 * entweder aus accept für einen socket oder mittels
 * fileno(stdout) für den stdout
 * Der zweite Parameter bestimmt das Format,
 * true => HTML Format; false => Textformat
 *************************************************************/
void printBuffer(int tn_socket, bool htmlformat);
/*************************************************************
 * Construktor des Buffers
 ************************************************************/
Gateway(void);

};

#endif // _GATEWAY_H_
