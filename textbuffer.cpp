#include "textbuffer.h"

void Textbuffer::addMessage(char* newMessage) {
    textbuffer_t* p_data = new textbuffer_t;
    if (p_data) {
        memset(p_data, 0, DEBUGSTRINGSIZE);
        sprintf(p_data->message,"%s", newMessage);
        newEntry((void*)p_data);
    } else {
        printf("Textbuffer::addMessage => p_data <%p>\n", p_data);
    }
    hasMessage = true;
}

void Textbuffer::printMessage(void) {
    char buf[] = TSBUFFERSTRING;
    void* p_buffer = nextEntry(NULL);
    textbuffer_t* p_data;
    while ( p_buffer ) {
        p_data = (textbuffer_t *)getDataPtr(p_buffer);
        if (p_data) {
            printf("%s%s\n", log_ts(buf),p_data->message);
            free(p_data);
            delEntry(p_buffer);
        } else {
            printf("Textbuffer::printMessage => p_data <%p>\n", p_data);
        }
        p_buffer = nextEntry(NULL);
    }
    hasMessage = false;
}

Textbuffer::Textbuffer(void) {
    hasMessage = false;
    isLocked=false;
}
