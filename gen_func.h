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
#include "node.h"
#include "sensor.h"
#include "order.h"
#include "orderbuffer.h"
#include "log.h"
#include <thread>

/*************************************************
 * 
 * This module covers all 
 * the generic functions
 * 
 ************************************************/
 
char * trim (char * s);
