/*


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
        uint8_t         pa_level;
        uint32_t        pa_utime;
        float			u_batt;	
        bool            is_HB_node;
        uint64_t        HB_ts;
        node_t*         p_next;
    };
node_t*     p_initial;
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
void    newEntry(node_t*);

public:

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
 *  Druckt den Inhalt des Buffers auf StdIO
 *************************************************************/
void printBuffer(void);
/**************************************************************
 *  Fügt einen neuen Node hinzu
 *************************************************************/    
void addNode(NODE_DATTYPE node_id, float u_batt, bool is_HB_node, uint8_t PALevel, uint32_t PAUtime );
/*************************************************************
 *  Prüft ob ein Heartbeat neu (>5000ms zum letzten Heartbeat) 
 *  oder alt ist.
 ************************************************************/
bool isNewHB(NODE_DATTYPE node_id, uint64_t mymillis);
/*************************************************************
 *  Prüft ob ein Node ein Heartbeat Node ist oder nicht 
 ************************************************************/
bool isHBNode(NODE_DATTYPE node_id);
/*************************************************************
 * Setzt den letzten ermittelten PA Level für diesen Node
 * PA Level 0=Min; 1=Low; 2=High; 3=Max; 9=unknown
 ************************************************************/
void setPaLevel(NODE_DATTYPE node_id, uint8_t pa_level);
/*************************************************************
 * Setzt die letzte gemessene Spannung des Nodes
 ************************************************************/
void setVoltage(NODE_DATTYPE node_id, float u_batt);
/*************************************************************
 * Git den Inhalt des Nodebuffers über den übergebenen 
 * telnet socket aus
 ************************************************************/
void printBuffer2tn(int tn_socket);
/*************************************************************
 * Construktor des Buffers
 ************************************************************/
Node(void);

};

#endif // _NODE_H_
