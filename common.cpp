#include "common.h"

char * trim (char * s) {
  /* Initialize start, end pointers */
  char *s1 = s, *s2 = &s[strlen (s) - 1];
  /* Trim and delimit right side */
  while ( (isspace (*s2)) && (s2 >= s1) )
    s2--;
  *(s2+1) = '\0';
  /* Trim left side */
  while ( (isspace (*s1)) && (s1 < s2) )
    s1++;
  /* Copy finished string */
  strcpy (s, s1);
  return s;
}

char* utime2str(time_t utime, char* buf, uint8_t format) {
    struct tm lt;
    const char* format;
    char res[22];
    if (utime == 0) utime = time(0);
    localtime_r(&utime, &lt);
    switch (form) {
        case 1:
            format = date_format1;
            break;
        case 2:
            format = date_format2;
            break;
    }            
    if (strftime(res, sizeof(res), format, &lt) == 0) {
        sprintf(buf,"Error");
    } else {
        sprintf(buf,"%s",res);
    }
    return buf;
}

char* ts(char* buf) {
    time_t now = time(0);
	tm *ltm = localtime(&now);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int msec=tv.tv_usec / 1000;
    sprintf(buf,"[%d.%s%d.%s%d %s%d:%s%d:%s%d.%s%u] ",ltm->tm_year + 1900, 
                    ltm->tm_mon<9? "0":"", ltm->tm_mon+1, ltm->tm_mday<10? "0":"", ltm->tm_mday,
                    ltm->tm_hour<10? " ":"", ltm->tm_hour, ltm->tm_min<10? "0":"", ltm->tm_min,
                    ltm->tm_sec<10? "0":"", ltm->tm_sec,
                    msec<10? "00":msec<100? "0":"", msec );
    return buf;
}

uint64_t mymillis(void) {
	struct timeval tv;
	uint64_t timebuf;
	gettimeofday(&tv, NULL);
	timebuf = (((tv.tv_sec & 0x000FFFFFFFFFFFFF) * 1000) + (tv.tv_usec / 1000));
	return timebuf;
}

char* printVerbose(uint16_t verboseLevel, char* buf) {
    if (verboseLevel & VERBOSECRITICAL )   sprintf(buf,"critical");
    if (verboseLevel & VERBOSESTARTUP )    sprintf(buf,"%s startup",buf);
    if (verboseLevel & VERBOSECONFIG )     sprintf(buf,"%s config",buf);
    if (verboseLevel & VERBOSETELNET )     sprintf(buf,"%s telnet",buf);
    if (verboseLevel & VERBOSERF24 )       sprintf(buf,"%s rf24",buf);
    if (verboseLevel & VERBOSESENSOR )     sprintf(buf,"%s sensor",buf);
    if (verboseLevel & VERBOSEOBUFFER )    sprintf(buf,"%s obuffer",buf);
    if (verboseLevel & VERBOSEOBUFFEREXT ) sprintf(buf,"%s obufferext",buf);
    if (verboseLevel & VERBOSEORDER )      sprintf(buf,"%s order",buf);
    if (verboseLevel & VERBOSEORDEREXT )   sprintf(buf,"%s orderext",buf);
    if (verboseLevel & VERBOSESQL )        sprintf(buf,"%s sql",buf);
    if (verboseLevel & VERBOSEOTHER )      sprintf(buf,"%s other",buf);
    if (verboseLevel & VERBOSEPOINTER )    sprintf(buf,"%s pointer",buf);
    return buf;
}

uint16_t decodeVerbose(uint16_t verboseLevel, char* verboseSet) {
    uint16_t retval = 0;
    char cmp_VBall[]="all",
         cmp_VBnone[]="none",
         cmp_addVBconf[]="+config",
         cmp_rmVBconf[]="-config",
         cmp_addVBtn[]="+telnet",
         cmp_rmVBtn[]="-telnet",
         cmp_addVBrf24[]="+rf24",
         cmp_rmVBrf24[]="-rf24",
         cmp_addVBsens[]="+sensor",
         cmp_rmVBsens[]="-sensor",
         cmp_addVBob[]="+obuffer",
         cmp_rmVBob[]="-obuffer",
         cmp_addVBobex[]="+obufferext",
         cmp_rmVBobex[]="-obufferext",
         cmp_addVBorder[]="+order",
         cmp_rmVBorder[]="-order",
         cmp_addVBorderex[]="+orderext",
         cmp_rmVBorderex[]="-orderext",
         cmp_addVBsql[]="+sql",
         cmp_rmVBsql[]="-sql",
         cmp_addVBother[]="+other",
         cmp_rmVBother[]="-other",
         cmp_addVBpnt[]="+pointer",
         cmp_rmVBpnt[]="-pointer";
    if (strcmp(verboseSet,cmp_VBall) == 0) {
        retval = VERBOSEALL;
    }
    if (strcmp(verboseSet,cmp_VBnone) == 0) {
        retval = VERBOSENONE;
    }
    if (strcmp(verboseSet,cmp_addVBconf) == 0) {
        retval = verboseLevel | VERBOSECONFIG;
    }
    if (strcmp(verboseSet,cmp_rmVBconf) == 0) {
        retval = verboseLevel ^ VERBOSECONFIG;
    }
    if (strcmp(verboseSet,cmp_addVBtn) == 0) {
        retval = verboseLevel | VERBOSETELNET;
    }
    if (strcmp(verboseSet,cmp_rmVBtn) == 0) {
        retval = verboseLevel ^ VERBOSETELNET;
    }
    if (strcmp(verboseSet,cmp_addVBrf24) == 0) {
        retval = verboseLevel | VERBOSERF24;
    }
    if (strcmp(verboseSet,cmp_rmVBrf24) == 0) {
        retval = verboseLevel ^ VERBOSERF24;
    }    
    if (strcmp(verboseSet,cmp_addVBsens) == 0) {
        retval = verboseLevel | VERBOSESENSOR;
    }
    if (strcmp(verboseSet,cmp_rmVBsens) == 0) {
        retval = verboseLevel ^ VERBOSESENSOR;
    }    
    if (strcmp(verboseSet,cmp_addVBob) == 0) {
        retval = verboseLevel | VERBOSEOBUFFER;
    }
    if (strcmp(verboseSet,cmp_rmVBob) == 0) {
        retval = verboseLevel ^ VERBOSEOBUFFER;
    }    
    if (strcmp(verboseSet,cmp_addVBobex) == 0) {
        retval = verboseLevel | VERBOSEOBUFFEREXT;
    }
    if (strcmp(verboseSet,cmp_rmVBobex) == 0) {
        retval = verboseLevel ^ VERBOSEOBUFFEREXT;
    }    
    if (strcmp(verboseSet,cmp_addVBorder) == 0) {
        retval = verboseLevel | VERBOSEORDER;
    }
    if (strcmp(verboseSet,cmp_rmVBorder) == 0) {
        retval = verboseLevel ^ VERBOSEORDER;
    }    
    if (strcmp(verboseSet,cmp_addVBorderex) == 0) {
        retval = verboseLevel | VERBOSEORDEREXT;
    }
    if (strcmp(verboseSet,cmp_rmVBorderex) == 0) {
        retval = verboseLevel ^ VERBOSEORDEREXT;
    }    
    if (strcmp(verboseSet,cmp_addVBsql) == 0) {
        retval = verboseLevel | VERBOSESQL;
    }
    if (strcmp(verboseSet,cmp_rmVBsql) == 0) {
        retval = verboseLevel ^ VERBOSESQL;
    }    
    if (strcmp(verboseSet,cmp_addVBother) == 0) {
        retval = verboseLevel | VERBOSEOTHER;
    }
    if (strcmp(verboseSet,cmp_rmVBother) == 0) {
        retval = verboseLevel ^ VERBOSEOTHER;
    }    
    if (strcmp(verboseSet,cmp_addVBpnt) == 0) {
        retval = verboseLevel | VERBOSEPOINTER;
    }
    if (strcmp(verboseSet,cmp_rmVBpnt) == 0) {
        retval = verboseLevel ^ VERBOSEPOINTER;
    }    
    return retval;
}


char* alloc_str(uint16_t verboselevel, const char* msgTxt, size_t size, char* timestamp) {
    if (verboselevel & VERBOSEPOINTER) {
        printf("%sMalloc %s P:", timestamp, msgTxt);
    }    
    char *retval =  (char*) malloc (size);
    memset(retval, 0, size);
    if (verboselevel & VERBOSEPOINTER) {
        printf("<%p> OK\n", retval);
    }    
    return retval;
}

void free_str(uint16_t verboseLevel, const char* msgTxt, char* str, char* timestamp) {
    if (verboseLevel & VERBOSEPOINTER) {
        printf("%sFree %s P:<%p>", timestamp, msgTxt, str);
    }    
    free(str);
    if (verboseLevel & VERBOSEPOINTER) {
        printf(" OK\n");
    }    
}

void sendUdpMessage(const char* host, const char* port, udpdata_t * udpdata ) {
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;   //AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	rv = getaddrinfo(host, port, &hints, &servinfo);
	if ( rv != 0 ) {
	fprintf(stderr, "Host: %s Port: %p \n", host, port);
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
//		exit(1);
	} else {
		// loop through all the results and make a socket
		for(p = servinfo; p != NULL; p = p->ai_next) {
			sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if ( sockfd == -1 ) {
				perror("Error: socket");
				continue;
			}
			break;
		}
		if (p == NULL) {
			fprintf(stderr, "Error: failed to bind socket\n");
			//exit(1);
		} else {
			if ((numbytes = sendto(sockfd, udpdata, sizeof(udpdata_t), 0, p->ai_addr, p->ai_addrlen)) == -1) {
				perror("Error: sendto error");
				//exit(1);
			}
		}
		if ( sockfd != -1 ) close(sockfd);
        }
	if ( rv == 0 ) freeaddrinfo(servinfo);
}

bool openSocket(const char* port, struct sockaddr_in *address, int* handle, sockType_t sockType ) {
    int in_socket = 0;
    int rv;
    bool retval = true;
    long save_fd;
	const int y = 1;
    struct addrinfo hints, *servinfo, *p;
//    hints.ai_family = AF_INET;  // force IPv4
//    hints.ai_family = AF_INET6; // force IPv6
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    if ( sockType == TCP ) {
		hints.ai_socktype = SOCK_STREAM;
	} else {
		hints.ai_socktype = SOCK_DGRAM;
	}		
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    rv = getaddrinfo(NULL, port, &hints, &servinfo);
    if (rv != 0) retval = false;
	// loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((in_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                continue;
        }
        if (bind(in_socket, p->ai_addr, p->ai_addrlen) == -1) {
           close(in_socket);
           continue;
        }
        break;
    }
    freeaddrinfo(servinfo);
	setsockopt( in_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int) );
	listen (in_socket, 5);
	save_fd = fcntl( in_socket, F_GETFL );
	save_fd |= O_NONBLOCK;
	fcntl( in_socket, F_SETFL, save_fd );
    *handle = in_socket;
    return retval;
}

void printPayload(char* ts, const char* header, payload_t* payload) {
    char *buf[6];
    for (int  i = 0; i < 6; i++) buf[i]=(char*)malloc(10);
    buf[0]=unpackTransportValue(payload->data1, buf[0]);
    buf[1]=unpackTransportValue(payload->data2, buf[1]);
    buf[2]=unpackTransportValue(payload->data3, buf[2]);
    buf[3]=unpackTransportValue(payload->data4, buf[3]);
    buf[4]=unpackTransportValue(payload->data5, buf[4]);
    buf[5]=unpackTransportValue(payload->data6, buf[5]);
    printf("%s%s N:%u T:%u m:%u F:0x%02X O:%u H:%u (%u/%s)(%u/%s)(%u/%s)(%u/%s)(%u/%s)(%u/%s)\n", ts, header,
               payload->node_id, payload->msg_type, payload->msg_id, payload->msg_flags, payload->orderno, payload->heartbeatno,
               getChannel(payload->data1), buf[0],
               getChannel(payload->data2), buf[1],
               getChannel(payload->data3), buf[2],
               getChannel(payload->data4), buf[3],
               getChannel(payload->data5), buf[4],
               getChannel(payload->data6), buf[5]);   
    for (int  i = 0; i < 6; i++) free(buf[i]);
}




