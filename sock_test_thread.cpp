/*
** server.c -- a stream socket server demo
**  uses threads; nonblocking; multiclients
**
**  compile:
**  g++ -std=c++0x -pthread -o socktest sock_test_thread.cpp  
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string> 
#include <thread>

#define TCP_PORT "7001"  // the port users will be connecting to for TCP
#define UDP_PORT "7002"  // the port users will be connecting to for UDP
#define BACKLOG 10     // how many pending connections queue will hold
    
    using namespace std;
    
    struct sensor_t {
	uint32_t 		network_id;
	uint32_t		msg_id;
	uint32_t		sensor_id;
	float			value;
};

void sigchld_handler(int s) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
    

void receive_tn_in(int new_fd) {
    ssize_t MsgLen;
    char buffer[20];
    memset(&buffer, 0, sizeof buffer);
    // send something like a prompt.
    send(new_fd, "telnet>", 7, 0);
    // and receive a message
    MsgLen = recv(new_fd, buffer, 20, 0);
    printf("Message: %s\n",buffer); 
    close(new_fd);
}

int main(void) {
	int numbytes;
    struct sensor_t sensordaten;
    int linecount = 0;
    int tcp_sockfd, udp_sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    int MsgLen;
    long tcp_save_fd, udp_save_fd;
	socklen_t addr_len;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    // open TCP Socket
    memset(&hints, 0, sizeof hints);
    hints.ai_family =  	AF_UNSPEC; //AF_INET; //AF_INET6; //AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    if ((rv = getaddrinfo(NULL, TCP_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        printf("Adressloop: \n");
        if ((tcp_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(tcp_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(tcp_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(tcp_sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo); // all done with this structure
    if (listen(tcp_sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    tcp_save_fd = fcntl( tcp_sockfd, F_GETFL );
	tcp_save_fd |= O_NONBLOCK;
	fcntl( tcp_sockfd, F_SETFL, tcp_save_fd );
    printf("server: waiting for tcp connections on %s ...\n", TCP_PORT);

    // open UDP Socket
    memset(&hints, 0, sizeof hints);
    hints.ai_family =  	AF_UNSPEC; //AF_INET; //AF_INET6; //AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    if ((rv = getaddrinfo(NULL, UDP_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        printf("Adressloop: \n");
        if ((udp_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (bind(udp_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(udp_sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo); // all done with this structure
    udp_save_fd = fcntl( udp_sockfd, F_GETFL );
	udp_save_fd |= O_NONBLOCK;
	fcntl( udp_sockfd, F_SETFL, udp_save_fd );
    printf("server: waiting for UDP connections on %s ...\n", UDP_PORT);

    while(1) {  // main accept() loop
        addr_len = sizeof their_addr;
        new_fd = accept(tcp_sockfd, (struct sockaddr *)&their_addr, &addr_len);
        if (new_fd > 0) {
            inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
            printf("server: got connection from %s\n", s);
            std::thread t2(receive_tn_in, new_fd);
            t2.detach();
        } 
        numbytes = recvfrom(udp_sockfd, &sensordaten, sizeof(sensordaten), 0, (struct sockaddr *)&their_addr,  &addr_len);
        if (numbytes >0) {
            printf("listener: got packet from %s\n",
            inet_ntop(their_addr.ss_family,	get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
            printf("listener: packet is %d bytes long\n", numbytes);
            printf("listener: packet received \n");
            printf("Network_number: %u \n",sensordaten.network_id);
            printf("Msg_number: %u \n",sensordaten.msg_id);
            printf("Sensor_id: %u \n",sensordaten.sensor_id);
            printf("Sensor_value: %f \n",sensordaten.value);
        }
        linecount++;
        printf(".");
        if ( linecount > 80 ) {
            linecount=0;
            printf("\n");
        }
        usleep(100000);
    }
    return 0;
}

