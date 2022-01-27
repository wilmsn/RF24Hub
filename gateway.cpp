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
        delete p_search;
        p_search = p_initial;
    }
}

bool Gateway::newEntry(gateway_t* p_new) {
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

bool Gateway::delEntry(gateway_t* p_del) {
    gateway_t *p_search, *p_tmp;
    bool retval = false;
    p_search = p_initial;
    p_tmp = p_initial;
    while (p_search) {
        if (p_search == p_del ) {
            free(p_search->gw_name);
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

void Gateway::delGateway(uint16_t gw_no) {
    gateway_t *p_search = p_initial;
    while (p_search) {
      if ( p_search->gw_no == gw_no ) {
        if (delEntry(p_search)) p_search = NULL;
      } else {
        p_search = p_search->p_next;
      }
    }
}

void Gateway::addGateway(char* gw_name, uint16_t gw_no, bool isActive) {
    gateway_t *p_new = new gateway_t;
    p_new->gw_name = (char*)malloc(40);
    p_new->gw_no = gw_no;
    p_new->last_contact = 0;
    sprintf(p_new->gw_name,"%s",gw_name);
    p_new->isActive = isActive;
    if (verboselevel & VERBOSESENSOR) printf("%sGateway.addGateway: Name:%s No:%u %s\n",ts(tsbuf), gw_name, gw_no, isActive? "aktiv":"nicht aktiv");
    newEntry(p_new);
}

void Gateway::setGateway(uint16_t gw_no, bool isActive ) {
    gateway_t *p_search;
    p_search = p_initial;
    while (p_search) {
        if (p_search->gw_no == gw_no) {
            if (verboselevel & VERBOSETELNET)
                printf("%sGateway.isGW: GW.Name:%s %s\n", ts(tsbuf), p_search->gw_name, p_search->isActive? "aktiv":"nicht aktiv");
            p_search->isActive = isActive;
            p_search = NULL;
        } else {
            p_search = p_search->p_next;
        }
    }
}

void Gateway::gw_contact(uint16_t gw_no){
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

void* Gateway::getGateway(void* p_rec, char* gw_name, uint16_t *p_gw_no) {
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
            sprintf(gw_name, "%s", p_search->gw_name);
            *p_gw_no = p_search->gw_no;
            retval = (void*)p_search;
            p_search = NULL;
        }
        if (p_search) p_search = p_search->p_next;
    }
    return retval;
}

bool Gateway::isGateway(uint16_t gw_no) {
    gateway_t *p_search;
    bool retval = false;
    p_search = p_initial;
    while (p_search) {
        if (p_search->gw_no == gw_no) {
            p_search->last_contact = time(NULL);
            if (verboselevel & VERBOSEORDER) 
                printf("%sGateway.isGW: GW.Name:%s %s\n", ts(tsbuf), p_search->gw_name, p_search->isActive? "aktiv":"nicht aktiv");
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
    char date[20];
    char ts[20];
    gateway_t *p_search;
    p_search = p_initial;
    sprintf(client_message," ------ Gateways: ------\n"); 
    write(out_socket , client_message , strlen(client_message));
    while (p_search) {
        sprintf(ts,"%s","\t");
        struct tm *tm = localtime(&p_search->last_contact);
        strftime(date, sizeof(date), "%d.%m.%Y %H:%M", tm);
        size_t nl = strlen(p_search->gw_name);
        //if (nl < 30) sprintf(ts,"%s%s",ts,"\t");
        if (nl < 24) sprintf(ts,"%s%s",ts,"\t");
        if (nl < 18) sprintf(ts,"%s%s",ts,"\t");
        if (nl < 12) sprintf(ts,"%s%s",ts,"\t");
        sprintf(client_message,"GW.Name:%s%s\tGW.NO: %u\t %s  Last: %s\n", p_search->gw_name, ts, p_search->gw_no, p_search->isActive? "aktiv      ":"nicht aktiv", date );
		write(out_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
	}
    free(client_message);
}

void Gateway::setVerbose(uint16_t _verboselevel) {
    verboselevel = _verboselevel;
}
