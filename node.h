/*


*/
#ifndef _NODE_H_   
#define _NODE_H_
#include <stdint.h>
#include "rf24hub_config.h"

class Node {

    
//private:
public:
    
struct node_t {
	uint16_t       	node;
    float			u_batt;	
    bool            is_HB_node;
	node_t*         next;          // poiter to the next record
};

    node_t *initial_ptr;
    void new_entry(node_t*);
    Node(void);

};

#endif // _NODE_H_
