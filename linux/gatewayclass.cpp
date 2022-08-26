#include "gatewayclass.h"

GatewayClass::GatewayClass(void) {
    p_initial = NULL;
    verboseLevel = 0;
    buf = (char*)malloc(TSBUFFERSIZE);
    tsbuf = (char*)malloc(TSBUFFERSIZE);
}

void GatewayClass::cleanup(void) {
    gateway_t *p_search;
    p_search = p_initial;
    while ( p_search ) {
        p_initial = p_search->p_next;
        free(p_search->gw_hostname);
        delete p_search;
        p_search = p_initial;
    }
}

bool GatewayClass::newEntry(gateway_t* p_new) {
    gateway_t *p_search;
    bool retval = false;
    p_new->p_next = NULL;
    if (p_initial) {
        p_search = p_initial;
        while (p_search->p_next) {
            p_search = p_search->p_next;
        }
        p_search->p_next = p_new;
        retval = true;
    } else {
        p_initial = p_new;
        retval = true;
    }
    return retval;
}

bool GatewayClass::delEntry(gateway_t* p_del) {
    gateway_t *p_search, *p_tmp;
    bool retval = false;
    p_search = p_initial;
    p_tmp = p_initial;
    while (p_search) {
        if (p_search == p_del ) {
            free(p_search->gw_hostname);
            if (p_search == p_initial) {
                if (p_initial->p_next) { 
                    p_tmp=p_initial->p_next;
                    delete p_initial;
                    p_initial=p_tmp;
                    retval = true;
                } else {
                    delete p_initial;
                    p_initial = NULL;
                    retval = true;
                }
            } else            {
                p_tmp->p_next=p_search->p_next;
                delete p_search;
                retval = true;
            }
            p_search = NULL;
        } else {
            p_tmp = p_search;
            p_search=p_search->p_next;
        }
    }
    return retval;
}

void GatewayClass::delGateway(uint16_t gw_no) {
    gateway_t *p_search = p_initial;
    while (p_search) {
      if ( p_search->gw_no == gw_no ) {
        if (delEntry(p_search)) p_search = NULL;
      } else {
        p_search = p_search->p_next;
      }
    }
}

void GatewayClass::addGateway(char* gw_hostname, uint16_t gw_no, bool isActive) {
    gateway_t *p_new = new gateway_t;
    p_new->gw_hostname = (char*)malloc(40);
    p_new->gw_no = gw_no;
    p_new->last_contact = 0;
    sprintf(p_new->gw_hostname,"%s",gw_hostname);
    p_new->isActive = isActive;
    if (verboseLevel & VERBOSESENSOR) printf("%sGateway.addGateway: Name:%s No:%u %s\n",ts(tsbuf), gw_hostname, gw_no, isActive? "aktiv":"nicht aktiv");
    newEntry(p_new);
}

void GatewayClass::setGateway(uint16_t gw_no, bool isActive ) {
    gateway_t *p_search;
    p_search = p_initial;
    while (p_search) {
        if (p_search->gw_no == gw_no) {
            if (verboseLevel & VERBOSETELNET)
                printf("%sGateway.isGW: GW.Name:%s %s\n", ts(tsbuf), p_search->gw_hostname, p_search->isActive? "aktiv":"nicht aktiv");
            p_search->isActive = isActive;
            p_search = NULL;
        } else {
            p_search = p_search->p_next;
        }
    }
}

void GatewayClass::gw_contact(uint16_t gw_no){
    gateway_t *p_search;
    p_search = p_initial;
    while (p_search) {
        if (p_search->gw_no == gw_no) {
            p_search->last_contact = time(NULL);
            p_search = NULL;
        }
        if (p_search) p_search = p_search->p_next;
    }
}

void* GatewayClass::getGateway(void* p_rec, char* gw_hostname, uint16_t *p_gw_no) {
    gateway_t *p_search;
    void* retval = NULL;
    if (p_rec) {
        p_search = (gateway_t*)p_rec;
        p_search = p_search->p_next;
    } else {
        p_search = p_initial;
    }
    while (p_search) {
        if (p_search->isActive && (p_search->last_contact > time(NULL)-3600)) {
            sprintf(gw_hostname, "%s", p_search->gw_hostname);
            *p_gw_no = p_search->gw_no;
            retval = (void*)p_search;
            p_search = NULL;
        }
        if (p_search) p_search = p_search->p_next;
    }
    return retval;
}

bool GatewayClass::isGateway(uint16_t gw_no) {
    gateway_t *p_search;
    bool retval = false;
    p_search = p_initial;
    while (p_search) {
        if (p_search->gw_no == gw_no) {
            p_search->last_contact = time(NULL);
            if (verboseLevel & VERBOSEORDER) 
                printf("%sGateway.isGW: GW.Name:%s %s\n", ts(tsbuf), p_search->gw_hostname, p_search->isActive? "aktiv":"nicht aktiv");
            if (p_search->isActive) retval = true;
            p_search = NULL;
        } else {
            p_search = p_search->p_next;
        }
    }    
    return retval;
}

void GatewayClass::printBuffer(int out_socket, bool htmlformat) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    char date[20];
//    char ts[20];
    char tb[5];
    char buf[30];
    gateway_t *p_search;
    p_search = p_initial;
    sprintf(client_message," ------ Gateways: ------\n"); 
    write(out_socket , client_message , strlen(client_message));
    while (p_search) {
        sprintf(tb,"%s","\t");
        struct tm *tm = localtime(&p_search->last_contact);
        strftime(date, sizeof(date), "%d.%m.%Y %H:%M", tm);
        size_t nl = strlen(p_search->gw_hostname);
        //if (nl < 30) sprintf(ts,"%s%s",ts,"\t");
        if (nl < 24) snprintf(tb,4,"%s","\t");
        if (nl < 18) snprintf(tb,4,"%s","\t\t");
        if (nl < 12) snprintf(tb,4,"%s","\t\t\t");
        sprintf(client_message,"GW.Name:%s%s\tGW.NO: %u\t %s  Last: %s\n", p_search->gw_hostname, tb, p_search->gw_no, p_search->isActive? "aktiv      ":"nicht aktiv", date );
		write(out_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
	}
    free(client_message);
}

void GatewayClass::setVerbose(uint16_t _verboseLevel) {
    verboseLevel = _verboseLevel;
}
