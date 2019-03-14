#include "rf24hub_common.h"
#include "config.h"
#include "DB-mariaDB.h"
#include "orderBuffer.h"

#include <sys/socket.h>
#include <stdio.h>
#include "rf24hub_common.h"
#include <stdlib.h>
#include <netinet/in.h>

using namespace std;

#define BUF 1024

extern CONFIG cfg;
extern SENSORBUFFER sensorbuffer;
extern ORDERBUFFER orderbuffer;
extern DB db;

void exec_tn_cmd(const char *tn_cmd);

void prepare_tn_cmd(uint16_t node, uint8_t channel, float value);

void process_tn_in(int new_tn_in_socket, char* buffer);

void receive_tn_in(int new_tn_in_socket, struct sockaddr_in * address);
