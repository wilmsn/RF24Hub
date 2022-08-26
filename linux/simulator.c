/* simulator.c
*
* A simulator for
* a) an ESP node
* b) a combination of rf24-node and gateway
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include "common.h"
#include "dataformat.h"
#include "config.h"
#include "rf24_config.h"
#define HUB_UDP_PORT "7004"
#define GW_UDP_PORT  "7003"


int main (int argc, char **argv) {
  udpdata_t udpdata;
  unsigned long starttime;
  ssize_t UdpMsgLen;
  socklen_t udp_addrlen;
  struct sockaddr_in udp_address_in;
  struct hostent *h;
  int udp_sockfd_in;
  payload_t payload;
  char tsbuf[30];
  char buf1[15];
  char hostaddr[64];

  /* Kommandozeile auswerten */
  if (argc != 7 && argc != 9 && argc != 11 && argc != 13 && argc != 15 && argc != 17 ) {
    printf ("Usage:(%d) %s <server> <gw_no> <ESP|RF24> <node_id> <channel1> <value1> [<channel2> <value2> [ ... [<channel6> <value6>]]] \n",
       argc, argv[0] );
    exit (EXIT_FAILURE);
  }

  /* IP-Adresse vom Server überprüfen */
  h = gethostbyname (argv[1]);
  if (h == NULL) {
    printf ("%s: unbekannter Host '%s' \n", argv[0], argv[1] );
    exit (EXIT_FAILURE);
  }
  unsigned int i=0;
  while ( h -> h_addr_list[i] != NULL) {
    sprintf(hostaddr, "%s", inet_ntoa( *( struct in_addr*)( h -> h_addr_list[i])));
    i++;
  }

  /* Socket erzeugen => incoming*/
  if ( ! openSocket(GW_UDP_PORT, &udp_address_in, &udp_sockfd_in, UDP) ) {
     printf("Error opening port: %s !!!!\n", GW_UDP_PORT);
     exit (0);
  }
  printf("%s Socket für eingehende UDP Messages auf Port %s angelegt\n", ts(tsbuf), GW_UDP_PORT);
  
  /* Daten fuellen */
  memset(&udpdata,0,sizeof(udpdata_t));
  udpdata.gw_no = atoi(argv[2]);
  udpdata.payload.node_id = atoi(argv[4]);
  if ( strcmp(argv[3],"RF24") == 0 ) udpdata.payload.msg_type = PAYLOAD_TYPE_HB;
  if ( strcmp(argv[3],"ESP") == 0 ) udpdata.payload.msg_type = PAYLOAD_TYPE_ESP;
  udpdata.payload.data1 = calcTransportValue(atoi(argv[5]),argv[6]);
  if (argc > 8) udpdata.payload.data2 = calcTransportValue(atoi(argv[7]),argv[8]); 
  if (argc > 10) udpdata.payload.data3 = calcTransportValue(atoi(argv[9]),argv[10]);
  if (argc > 12) udpdata.payload.data4 = calcTransportValue(atoi(argv[11]),argv[12]);
  if (argc > 14) udpdata.payload.data5 = calcTransportValue(atoi(argv[13]),argv[14]);
  if (argc > 16) udpdata.payload.data6 = calcTransportValue(atoi(argv[15]),argv[16]);

  /* Daten senden */
  printf("%s Sende Daten an: %s (%s:%s)\n",ts(tsbuf), argv[1], hostaddr, HUB_UDP_PORT);
  sprintf(buf1,"G:%u>H ", udpdata.gw_no);
  printPayload(ts(tsbuf), buf1, &udpdata.payload);
  sendUdpMessage(argv[1], HUB_UDP_PORT, &udpdata);

  /* Reaktion abwarten */
  starttime = mymillis();
  while (starttime > mymillis() - 1000) {
    UdpMsgLen = recvfrom ( udp_sockfd_in, &udpdata, sizeof(udpdata_t), 0, (struct sockaddr *) &udp_address_in, &udp_addrlen );
    if (UdpMsgLen > 0) {
        memset(&buf1,0,sizeof(buf1));
        memcpy(&payload, &udpdata.payload, sizeof(payload) );
        printf ("%s UDP Message from: %s \n", ts(tsbuf), inet_ntoa(udp_address_in.sin_addr));
        printPayload(ts(tsbuf), "H>G ", &payload);
    }
  }
  printf("Programmende \n");
  return EXIT_SUCCESS;
}
