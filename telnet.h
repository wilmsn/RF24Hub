#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "rf24hub_common.h"
#include "rf24hub_config.h"
#include "config.h"
#include <thread>

/*************************************************
 * 
 * This module covers all 
 * the communication comeing in via socket
 * 
 ************************************************/
 
extern CONFIG cfg;
extern char debug[DEBUGSTRINGSIZE]; 
extern sockType_t sockType;

void openSocket(const char* host, const char* port, struct sockaddr_in *address, int* handle, sockType_t sockType );

void receiveTelnetMessage(int tn_socket, struct sockaddr_in * address);

void sendUdpMessage(const char* host, const char* port, struct udp_data_t * udp_data );
