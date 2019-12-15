#include "telnet.h"




void sendUdpMessage(const char* host, const char* port, udp_data_t * udp_data ) {
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		exit(1);
	}
	if ((numbytes = sendto(sockfd, udp_data, sizeof(udp_data_t), 0, p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto error");
		exit(1);
	}
	close(sockfd);
    freeaddrinfo(servinfo);
}


void openSocket(const char* host, const char* port, struct sockaddr_in *address, int* handle, sockType_t sockType ) {
    int in_socket;
    int rv;
    long save_fd;
	const int y = 1;
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    cout << host << ":" << port << endl;
//   	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
//    hints.ai_family = AF_INET6; // set to AF_INET to force IPv4
    hints.ai_family = AF_INET; // set to AF_INET to force IPv4
    if ( sockType == TCP ) {
		hints.ai_socktype = SOCK_STREAM;
	} else {
		hints.ai_socktype = SOCK_DGRAM;
	}		
    hints.ai_flags = AI_PASSIVE; // use my IP
    if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
        //return (int)-1;
    }
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
    if (p == NULL) {
        //return (int)-1;
    }
    freeaddrinfo(servinfo);
	setsockopt( in_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int) );
	listen (in_socket, 5);
	save_fd = fcntl( in_socket, F_GETFL );
	save_fd |= O_NONBLOCK;
	fcntl( in_socket, F_SETFL, save_fd );
    *handle = in_socket;
}

