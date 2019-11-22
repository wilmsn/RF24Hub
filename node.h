/*


*/
#ifndef _NODE_H_   
#define _NODE_H_
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include "log.h"
#include "rf24hub_config.h"

class Node {

    
private:
    Logger* logger;
    struct node_t {
        uint16_t       	node;
        float			u_batt;	
        bool            is_HB_node;
        uint64_t        HB_ts;
        node_t*         next;
    };
    node_t *initial_ptr;
    void new_entry(node_t*);

public:

    void add_node(uint16_t node, float u_batt, bool is_HB_node );
    bool is_new_HB(uint16_t node, uint64_t mymillis);
    bool is_HB_node(uint16_t mynode);
    void print_buffer2tn(int new_tn_in_socket);
    void print_buffer2log(void);
    void begin(Logger* _logger);
    Node(void);

};

#endif // _NODE_H_
