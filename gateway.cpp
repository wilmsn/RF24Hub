#include "gateway.h"

Gateway::Gateway(void) {
    p_initial = NULL;
    verboselevel = 0;
    buf = (char*)malloc(TSBUFFERSIZE);
    tsbuf = (char*)malloc(TSBUFFERSIZE);
}

void Gateway::cleanup(void) {
    gateway_t *p_search;
    p_search = p_initial;
    while ( p_search ) {
        p_initial = p_search->p_next;
        free(p_search->gw_name);
        free(p_search->gw_ip);
        delete p_search;
        p_search = p_initial;
    }
}

void Gateway::newEntry(gateway_t* p_new) {
    gateway_t *p_search;
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

void Gateway::addGateway(char* gw_name, char* gw_ip, uint16_t gw_no, bool isActive) {
    gateway_t *p_new = new gateway_t;
    p_new->gw_name = (char*)malloc(40);
    p_new->gw_no = gw_no;
    p_new->gw_ip = (char*)malloc(40);
    sprintf(p_new->gw_name,"%s",gw_name);
    sprintf(p_new->gw_ip,"%s",gw_ip);
    p_new->isActive = isActive;
    if (verboselevel & VERBOSESENSOR) printf("%sGateway.addGateway: Name:%s IP:%s %s\n",ts(tsbuf), gw_name, gw_ip, isActive? "aktiv":"nicht aktiv");
    newEntry(p_new);
}

void* Gateway::getGW(void* p_rec, char* gw_ip, uint16_t *p_gw_no) {
    gateway_t *p_search;
    void* retval = NULL;
    if (p_rec) {
        p_search = (gateway_t*)p_rec;
        p_search = p_search->p_next;
    } else {
        p_search = p_initial;
    }
    while (p_search) {
        if (p_search->isActive) {
            sprintf(gw_ip, "%s", p_search->gw_ip);
            *p_gw_no = p_search->gw_no;
            retval = (void*)p_search;
            p_search = NULL;
        }
        if (p_search) p_search = p_search->p_next;
    }
    return retval;
}

bool Gateway::isGateway(char* gw_ip) {
    gateway_t *p_search;
    bool retval = false;
    p_search = p_initial;
    while (p_search) {
        if (strcmp(p_search->gw_ip, gw_ip) == 0) {
            if (verboselevel & VERBOSEORDER) 
                printf("%sGateway.isGW: GW.Name:%s GW.IP: %s %s\n", ts(tsbuf), p_search->gw_name, p_search->gw_ip, p_search->isActive? "aktiv":"nicht aktiv");
            if (p_search->isActive) retval = true;
            p_search = NULL;
        } else {
            p_search = p_search->p_next;
        }
    }    
    return retval;
}

void Gateway::printBuffer(int out_socket, bool htmlformat) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    gateway_t *p_search;
    p_search = p_initial;
    sprintf(client_message," ------ Gateways: ------\n"); 
    write(out_socket , client_message , strlen(client_message));
    while (p_search) {
        sprintf(client_message,"GW.Name:%s GW.NO %u GW.IP: %s %s\n", p_search->gw_name, p_search->gw_no, p_search->gw_ip, p_search->isActive? "aktiv":"nicht aktiv" );    
		write(out_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
	}
    free(client_message);
}

void Gateway::setVerbose(uint16_t _verboselevel) {
    verboselevel = _verboselevel;
}
