#include "rf24hub_common.h"
#include "config.h"

#include <sys/socket.h>
#include <stdio.h>
#include "rf24hub_common.h"
#include <stdlib.h>
#include <netinet/in.h>

#define BUF 1024

extern CONFIG cfg;

void exec_tn_cmd(const char *tn_cmd);

void prepare_tn_cmd(uint16_t node, uint8_t channel, float value);

void process_tn_in(int new_tn_in_socket, char* buffer, char* client_message);

void receive_tn_in(int new_tn_in_socket, struct sockaddr_in * address);
