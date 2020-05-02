#include "buffer.h"

Buffer::Buffer(void) {
    p_initial = NULL;
    isLocked = false;
}

void Buffer::newEntry(void* p_data_new) {
    if (isLocked) usleep(100);
    isLocked = true;
    buffer_t *p_search;
    Buffer::buffer_t* p_new = new buffer_t; 
    if (p_new) {
        p_new->p_data = p_data_new;
        p_new->p_next = NULL;
        if (p_initial) {
            p_search = p_initial;
            while (p_search->p_next) {
                p_search = p_search->p_next;
            }
            p_search->p_next = p_new;
        } else {
            p_initial = p_new;
        }
    }
    isLocked = false;
}

bool Buffer::delEntry(void* p_buffer_in) {
    if (isLocked) usleep(100);
    isLocked = true;
    bool retval = false;
    buffer_t *p_buffer, *p_buffer_tmp, *p_buffer_del;
    p_buffer_del = (buffer_t*) p_buffer_in;
    p_buffer = p_initial;
    p_buffer_tmp = p_initial;
    while (p_buffer) {
        if (p_buffer == p_buffer_del ) {
            if (p_buffer == p_initial) {
                if (p_initial->p_next) { 
                    p_buffer_tmp = p_initial->p_next;
                    delete p_buffer;
                    p_initial = p_buffer_tmp;
                } else {
                    delete p_buffer;
                    p_initial = NULL;
                }
            } else            {
                p_buffer_tmp->p_next = p_buffer->p_next;
                delete p_buffer;
            }
            p_buffer = NULL;
            retval = true;
        } else {
            p_buffer_tmp = p_buffer;   // Ist das nötig ??????
            p_buffer = p_buffer->p_next;
        }
    }
    return retval;
    isLocked = false;
}

void* Buffer::nextEntry(void* p_findbuffer_void) {
    buffer_t *p_search, *p_result, *p_findbuffer;
    p_findbuffer = (buffer_t*)p_findbuffer_void;
    p_search = p_initial;
    if ( p_findbuffer ) {
        while ( p_search != p_findbuffer && p_search->p_next ) {  
            p_search = p_search->p_next;
        }
        p_result = p_search->p_next;
    } else {
        p_result = p_initial;
    }
    return (void*) p_result;    
}

void* Buffer::getDataPtr(void* p_buffer_void) {
    buffer_t* p_buffer;
    if (p_buffer_void) {
        p_buffer = (buffer_t *)p_buffer_void;
        return p_buffer->p_data;
    } else {
        return NULL;
    }
}
