/*


*/
#ifndef _NODE_H_   
#define _NODE_H_
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include "rf24hub_config.h"
#include "common.h"

extern uint16_t verboselevel;   

class Node {

    
private:

struct node_t {
        uint8_t       	node_id;
        uint8_t         pa_level;
        char            pa_level_datestr[20];
        float			u_batt;	
        bool            is_HB_node;
        uint64_t        HB_ts;
        node_t*         p_next;
};
    
node_t* p_initial;
void newEntry(node_t*);
void printError(const char* txt, node_t* pointer);

public:

/**************************************************************
 *  Löscht den kompletten Inhalt und leert den Buffer
 *************************************************************/    
void cleanup(void);
/**************************************************************
 *  Druckt den Inhalt des Buffers auf StdIO
 *************************************************************/
void printBuffer(uint16_t debuglevel);
/**************************************************************
 *  Fügt einen neuen Node hinzu
 *************************************************************/    
void addNode(uint8_t node_id, float u_batt, bool is_HB_node, uint8_t pa_level );
/*************************************************************
 *  Prüft ob ein Heartbeat neu (>5000ms zum letzten Heartbeat) 
 *  oder alt ist.
 ************************************************************/
bool isNewHB(uint8_t node_id, uint64_t mymillis);
/*************************************************************
 *  Prüft ob ein Node ein Heartbeat Node ist oder nicht 
 ************************************************************/
bool isHBNode(uint8_t node_id);

bool isValidNode(uint8_t node_id);

void setPaLevel(uint8_t node_id, uint8_t pa_level);

void setVoltage(uint8_t node_id, float u_batt);

void printBuffer2tn(int new_tn_in_socket);

Node(void);

};

#endif // _NODE_H_
