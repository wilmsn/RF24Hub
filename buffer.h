/*


*/
#ifndef _BUFFER_H_   
#define _BUFFER_H_
#include <stdio.h> 
#include <iostream>
#include <stdint.h>
#include <cstring>
#include <unistd.h>

class Buffer {
    
private:

    struct buffer_t {
        uint64_t		entrytime;
        void*     	    p_data;
        buffer_t*       p_next;          // poiter to the next record
    };
    buffer_t* p_initial;

protected:
    
public:

    bool isLocked;
    void newEntry(void*);
    bool delEntry(void*);
    void* nextEntry(void*);
    void* getDataPtr(void*);
    Buffer(void);
    
};

#endif // _BUFFER_H_
