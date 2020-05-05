#include "textbuffer.h"

void Textbuffer::newEntry(textbuffer_t* p_new_buffer) {
    if (isLocked) usleep(100);
    isLocked = true;
    textbuffer_t *p_search;
    if (p_new_buffer) {
        if (p_initial) {
            p_search = p_initial;
            while (p_search->p_next) {
                p_search = p_search->p_next;
            }
            p_search->p_next = p_new_buffer;
        } else {
            p_initial = p_new_buffer;
        }
    }
    isLocked = false;
}

bool Textbuffer::delEntry(textbuffer_t* p_buffer_del) {
    if (isLocked) usleep(100);
    isLocked = true;
    bool retval = false;
    textbuffer_t *p_buffer, *p_buffer_tmp;
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

void Textbuffer::addMessage(char* newMessage) {
    textbuffer_t* p_buffer = new textbuffer_t;
    if (p_buffer) {
        memset(p_buffer->message, 0, DEBUGSTRINGSIZE);
        sprintf(p_buffer->message,"%s", newMessage);
        p_buffer->p_next = NULL;
        newEntry(p_buffer);
    } else {
        printf("Textbuffer::addMessage => p_buffer <%p>\n", p_buffer);
    }
    hasMessage = true;
}

void Textbuffer::printMessage(void) {
    char buf[] = TSBUFFERSTRING;
    textbuffer_t* p_buffer = p_initial;
    while ( p_buffer ) {
        printf("%s%s\n", log_ts(buf),p_buffer->message);
        delEntry(p_buffer);
        p_buffer = p_buffer->p_next;
    }
    hasMessage = false;
}

Textbuffer::Textbuffer(void) {
    hasMessage = false;
    isLocked=false;
}
