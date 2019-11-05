/************************************************************************
*									*
*  telar - dual stack telnet autoresponder				*
*									*
*  This code demonstrates how to accept connections over either IPv4	*
*  or IPv6, in a single process. It uses non-blocking sockets. Once	*
*  a connection is accepted, a StreamCon data structure is created and	*
*  all further I/O is accomplished using routines from SCIO.		*
*									*
*  Author:	Lawrence E. Hughes					*
*  History:	v1.0 - 24 April 2011					*
*									*
*  Platform:	Tested on FreeBSD 8.2 and Ubuntu 10.10			*
*									*
*  Copyright (c) 2011, Lawrence E. Hughes				*
*  All Rights Reserved Worldwide					*
*  Released under Simplified BSD license				*
*  www.opensource.org/licenses/bsd-license.php				*
*									*
*  This code creates two stream mode sockets, one for IPv4 and one for	*
*  IPv6. It does not use a single IPv6 socket and IPv4-mapped IPv6	*
*  addresses. Both sockets are put into non-blocking mode, and then	*
*  put into listen mode. The select() function is used to determine	*
*  when a connection to either listening socket happens. Anytime a	*
*  connection is made, a child process is spawned to handle that	*
*  connection. The non-blocking socket is changed to blocking, a	*
*  StreamCon data structure is created, and routines in SCIO are used	*
*  to process the connection. 						*
*									*
*  This code is not meant to illustrate the optimal way to handle	*
*  multiple connections, and has not been designed for use in a thread	*
*  model server, but it should not be difficult to change it to use	*
*  a thread to process a connection instead of a process.		*
*  It is only intended to show how to use non-blocking sockets and the	*
*  select() routine to handle incoming connections over both IPv4 and	*
*  IPv6 in a single listening process.					*
*									*
*  To build this code, put telar.c, SCIO.c and SCIO.h in a single	*
*  directory, and issue the following command:				*
*									*
*	# gcc -o telar telar.c SCIO.c					*
*									*
*  This will create an executable on file "telar"			*
*									*
*  Since this tries to open a socket on port 23 (which is < 1024) you	*
*  will need to have root privilege to run it. You can launch it with	*
*									*
*	# ./telar &							*
*									*
*  To keep the code simple, no provision has been made to start, stop	*
*  or restart the process. To kill it, use ps to determine the process	*
*  id and kill it, e.g.							*
*									*
*  	# ps -ax | grep telar						*
*	 1406   1  S     0:00.00 ./telar				*
*       # kill 1406							*
*									*
************************************************************************/

/***********************
* HEADER FILE INCLUDES *
***********************/

#include "SCIO.h"

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <resolv.h>


/**************
* DEFINITIONS *
**************/

#ifndef FALSE
#define	FALSE	0
#endif

#ifndef	TRUE
#define TRUE	1
#endif

#define MAXLINE		1024
#define	MAX_MXREC	20
#define BACKLOG 	10	/* number of queued connections */


/************************
* FUNCTION DECLARATIONS *
*************************/

int handle_conn(int csd, int af, char *szAddress, char *szService);
void *get_in_addr(struct sockaddr *sa);
void sigchld_handler(int s);


/***********************
* FUNCTION DEFINIITONS *
***********************/

/************************************************************************
*									*
*  main - main program of telnet autoresponder (process model)		*
*									*
*  Accept connections over IPv4 or IPv6, and reply to the client with	*
*  the IP family and IP address from which they connected, then drop	*
*  the connection.							*
*									*
************************************************************************/

int main(int argc, char *argv[])
{
    int sd4, sd6;	// listen sockets for both v4 and v6
    int csd4, csd6;	// client sockets for both v4 and v6
    int rc, rv, opts, i, maxsd;
    int yes;

    char s[INET6_ADDRSTRLEN], szService[256];

    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    fd_set master_set, working_set;

    fprintf(stderr, "Process Model Dual Stack telnet autoresponder\n");

    /* default port if no arg, default is 23 (telnet) */
    strcpy(szService, "23");
    if (argc >= 2)
	strcpy(szService, argv[1]);

    /* prepare hints structure and call getaddrinfo() to resolve nodename */

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;	 // use my IP

    rv = getaddrinfo(NULL, szService, &hints, &servinfo);
    if (rv != 0)
    {
	fprintf(stderr, "getaddrinfo() failed, %s\n", gai_strerror(rv));
	exit(1);
    }

    sd4 = sd6 = 0;	/* no sockets yet */

    /* loop through all results from DNS and bind to first v4 and first v6 */
    for(p = servinfo; p != NULL; p = p->ai_next) 
    {
	/* is this address IPv4? */
	if (p->ai_family == AF_INET)
	{
	    /* no IPv4 listening socket yet? */
	    if(sd4 == 0)
	    {
		/* create one now */
 		sd4 = socket(p->ai_family, p->ai_socktype, p->ai_protocol); 
		if (sd4 == -1)
		{
		    fprintf(stderr, "socket(IPv4) failed, %s\n", strerror(errno));
		    sd4 = 0;
		    continue;
		}

		/* make IPv4 listening socket reusable */
		yes = 1;
		rc = setsockopt(sd4, SOL_SOCKET, SO_REUSEADDR, &yes, 
		    sizeof(int));
		if (rc == -1) 
		{
		    fprintf(stderr, "setsockopt() failed, %s\n", strerror(errno));
		    continue;
		}

		/* make IPv4 listening socket non-blocking */
		opts = fcntl(sd4, F_GETFL);
		if (opts < 0)
		{
		    fprintf(stderr, "fcntl(F_GETFL) failed\n");
		    continue;
		}
		
		opts = (opts | O_NONBLOCK);
		if (fcntl(sd4, F_SETFL, opts) < 0)
		{
		    fprintf(stderr, "fcntl(F_SETFL) failed\n");
		    continue;
		}

		/* bind IPv4 listening socket to IPv4 address*/
		rc = bind(sd4, p->ai_addr, p->ai_addrlen);
		if (rc == -1)
		{
		    close(sd4);
		    fprintf(stderr, "bind(IPv4) failed: %s\n", strerror(errno));
		    continue;
		}
	    }
	}
	else
	{
	    /* no IPv6 listening socket yet? */
	    if(sd6 == 0)
	    {

		/* make one now */
		sd6 = socket(p->ai_family, p->ai_socktype, p->ai_protocol); 
		if (sd6 == -1)
		{
		    fprintf(stderr, "socket(IPv6) failed, %s\n", strerror(errno));
		    sd6 = 0;
		    continue;
		}

		/* make IPv6 listening socket reusable */
		yes = 1;
		rc = setsockopt(sd6, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		if (rc == -1) 
		{
		    fprintf(stderr, "setsockopt(IPv6) failed, %s\n", strerror(errno));
		    continue;
		}

		/* make IPv6 listening socket IPv6 only */
		yes = 1;
		rc = setsockopt(sd6, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof(yes));
		if (rc == -1)
		{
		    fprintf(stderr, "setsockopt(IPV6_V6ONLY) failed, %s", strerror(errno));
		    continue;
		}

		/* make IPv6 listening socket non-blocking */
		opts = fcntl(sd6, F_GETFL);
		if (opts < 0)
		{
		    fprintf(stderr, "fcntl(F_GETFL) failed\n");
		    continue;
		}
		
		opts = (opts | O_NONBLOCK);
		if (fcntl(sd6, F_SETFL, opts) < 0)
		{
		    fprintf(stderr, "fcntl(F_SETFL) failed\n");
		    continue;
		}

		/* bind IPv6 listening socket to IPv6 address*/
		rc = bind(sd6, p->ai_addr, p->ai_addrlen);
		if (rc == -1)
		{
		    close(sd6);
		    fprintf(stderr, "bind(IPv6) failed: %s\n", strerror(errno));
		    continue;
		}
	    }
	}
    }

    /* error if unable to create any listening socket */
    if (sd4 == 0 && sd6 == 0)  
    {
	fprintf(stderr, "unable to bind to either IPv4 or IPv6 address\n");
	exit(1);
    }

    /* deallocate memory from getaddrinfo() */
    freeaddrinfo(servinfo);

    /* listen on IPv4 (non-blocking) */
    if (sd4 != 0)
    {
	if (listen(sd4, BACKLOG) == -1) 
	{
	    fprintf(stderr, "listen(IPv4) failed, %s\n", strerror(errno));
	    exit(1);
        }
	fprintf(stderr, "listening on IPv4\n");
    }

    /* listen on IPv6 (non-blocking) */
    if (sd6 != 0)
    {
	if (listen(sd6, BACKLOG) == -1) 
	{
	    fprintf(stderr, "listen(IPv6) failed, %s\n", strerror(errno));
	    exit(1);
        }
	fprintf(stderr, "listening on IPv6\n");
    }

    /* reap all dead processes */
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) 
    {
	fprintf(stderr, "sigaction() failed, %s\n", strerror(errno));
	exit(1);
    }

    fprintf(stderr, "Process model server: waiting for connections...\n");

    /* set up for select statement */
    FD_ZERO(&master_set);
    FD_SET(sd4, &master_set);
    FD_SET(sd6, &master_set);

    /* main loop - await connections */
    while(1) 
    {
	/* copy master sd_set to the working sd_set */
	memcpy(&working_set, &master_set, sizeof(master_set));

	/* maxsd = greater of sd4 or sd6 */
	maxsd = sd4;
	if (sd6 > maxsd)
	    maxsd = sd6;

	/* call select() to wait for connection from either listening socket */
	rc = select(maxsd + 1, &working_set, NULL, NULL, NULL);
	if (rc < 0)
	{
	    /* if error is EINTR, ignore it and go back to waiting */
	    if (errno == EINTR)
		continue;

	    /* else real error, abort */
	    fprintf(stderr, "select() failed, %s\n", strerror(errno));
	    exit(1);
	}

	/* if we get here, we have one or more connections! */

 	/* check for connection on IPv4 */
	if (FD_ISSET(sd4, &working_set))
	{
	    /* we have a winner on IPv4 - accept it and make a baby! */
	    sin_size = sizeof(their_addr);
	    csd4 = accept(sd4, (struct sockaddr *)&their_addr, &sin_size);
	    if (csd4 == -1) 
	    {
		fprintf(stderr, "accept() failed, %s\n", strerror(errno));
		continue;
	    }

	    /* csd4 inherited non-blocking status, make it blocking */
	    opts = fcntl(csd4, F_GETFL);
	    if (opts < 0)
	    {
		fprintf(stderr, "fcntl(F_GETFL) failed\n");
		continue;
	    }
		
	    opts = (opts & (~O_NONBLOCK));
	    if (fcntl(csd4, F_SETFL, opts) < 0)
	    {
		fprintf(stderr, "fcntl(F_SETFL) failed\n");
		continue;
	    }

	    /* display address of client */
	    inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr), s, sizeof(s));
	    fprintf(stderr, "IPv4 connection from %s\n", s);

	    /* in order to make a child, you have to fork()! */
	    if (!fork()) 
	    {
		/* this is the child process */

		/* close parent's socket */
		close(sd4);

		/* handle the connection */
		handle_conn(csd4, AF_INET, s, szService);

		/* close the child socket */
		close(csd4);

		/* child has completed its task, abort it */
		exit(0);
	    }
	    else
	    {
		/* this is the parent process */

		/* close the child socket and continue living */
		close(csd4);
	    }
	}

 	/* check for connection on IPv6 */
	if (FD_ISSET(sd6, &working_set))
	{
	    /* we have a winner on IPv6 - accept it and make a baby! */
	    sin_size = sizeof(their_addr);
	    csd6 = accept(sd6, (struct sockaddr *)&their_addr, &sin_size);
	    if (csd6 == -1) 
	    {
		fprintf(stderr, "accept() failed, %s\n", strerror(errno));
		continue;
	    }

	    /* csd6 inherited non-blocking status, make it blocking */
	    opts = fcntl(csd6, F_GETFL);
	    if (opts < 0)
	    {
		fprintf(stderr, "fcntl(F_GETFL) failed\n");
		continue;
	    }
		
	    opts = (opts & (~O_NONBLOCK));
	    if (fcntl(csd6, F_SETFL, opts) < 0)
	    {
		fprintf(stderr, "fcntl(F_SETFL) failed\n");
		continue;
	    }

	    /* display address of client */
	    inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr), s, sizeof(s));
	    fprintf(stderr, "IPv6 connection from %s\n", s);

	    /* to make a child, you have to fork()! */
	    if (!fork()) 
	    {
		/* this is the child process */

		/* close parent's socket */
		close(sd6);

		/* handle the connection */
		handle_conn(csd6, AF_INET6, s, szService);

		/* child has completed its task, abort it */
		exit(0);
	    }
	    else
	    {
		/* this is the parent process */

		/* close the child socket and continue living */
		close(csd6);
	    }
	}
    }

    /* normal exit */
    exit(0);
}


/************************************************************************
*									*
* handle_conn - handle incoming connection				*
*									*
*   int		csd		client socket descriptor (already open)	*
*   int		af		address family (AF_INET, AF_INET6)	*
*   char	*szAddress	client address, as string		*
*   char	*szService	service name or number, as string	*
*									*
*									*
*   returns	0		successful completetion			*
*		-1		failure					*
*									*
************************************************************************/

int handle_conn(int csd, int af, char *szAddress, char *szService)
{
    FILE *temp;

    int i, j, nbytes, rc, State;

    char szOutbuf[1024];
    char szError[1024];
    char szAddrFamily[8];

    StreamCon scd;

    /* initialize Stream Connection structure */
    rc = SCIO_init(&scd, 1024, szError);
    if (rc < 0)
    {
	fprintf(stderr, "SCIO_init() failed, %s\n", szError);
	return -1;
    }

    /* plug in fields to use connected client socket with SCIO library */
    scd.sd = csd;
    scd.bufptr = scd.buffer;
    scd.bufcount = 0;

    if (af == AF_INET)
	strcpy(szAddrFamily, "IPv4");
    else
	strcpy(szAddrFamily, "IPv6");

    sprintf(szOutbuf, "You connected over %s from %s\r\n", 
	szAddrFamily, szAddress);
    SCIO_putline(&scd, szOutbuf);

    strcpy(szOutbuf, "Goodbye!\r\n");
    SCIO_putline(&scd, szOutbuf);

    /* all done - return and abort child */
    return 0;
}    

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

/************************************************************************
*									*
*  get_in_addr - get sockaddr, IPv4 or IPv6				*
*									*
************************************************************************/

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) 
    { // return IPv4 address
	return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    else
    { // return IPv6 address
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
}

