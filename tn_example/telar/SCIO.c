/************************************************************************
*  SCIO.c - Stream Connection Input/Output library			*
*									*
*	Author:		Lawrence E. Hughes				*
*	Version:	1.0.1 - 24 May 2011				*
*	Platform(s):	Tested on FreeBSD 8.2, Ubuntu Linux 10.10	*
*	Rights:		Copyright (c) 2011, Lawrence E. Hughes		*
*			All Rights Reserved, Worldwide			* 
*			Released under Simplified BSD License		*
*			www.opensource.org/licenses/bsd-license.php     *
*									*
* This code is intended to demonstrate how a dual stack client can      *
* make connections to servers running IPv4 and/or IPv6, either serially *
* or in parallel. It uses non-blocking sockets. You can use this code   *
* as is, or incorporate parts of it, in your own projects. If you port  *
* it to other platforms (and verify correct operation), I would very    *
* much appreciate your releasing it also under BSD (not GPL) license.   *
* If you send me a copy of any ports to other platforms I will add it   *
* to my site for general distribution.                                  *
*									*
* The parallel connect routine currently connects to all addresses that *
* DNS returns, in parallel (you could modify that to connect only to    *
* the first IPv4 and first IPv6 address returned if you like). You can  *
* specify the maximum number of milliseconds to wait for a connection   *
* over IPv6, and the maximum number of millisecond before giving up.    *
* This solves the problem that prevents sites like www.google.com from  *
* advertising both A and AAAA records for their main URL. If most       *
* clients use this approach, it should minimize mysterious long delays  *
* from clients that first try over IPv6 then fail back to IPv4 after    *
* 30 to 70 second timeouts. 						*
*									*
************************************************************************/

// #define debugout  1	/* comment out for normal use */

#include "SCIO.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <fcntl.h>

extern errno;

/************************************************************************
*									*
*  SCIO_init - initialze StreamCon structure, allocate input buffer	*
*									*
*  You should call this routine first, with the desired size of your	*
*  input buffer. Be sure to call SCIO_free when done to release the	*
*  the memory allocated by this routine.				*
*									*
*    StreamCon	*scd		StreamCon structure			*
*    int	bufsize		size of input buffer, in bytes		*
*    char	*szError	returned error message			*
*									*
*  Returned value (int)							*
*									*
*    -1		error (message in szError)				*
*    0		success							*
*									*
************************************************************************/

int SCIO_init(StreamCon *scd, int bufsize, char *szError)
{
    /* clear StreamCon structure */
    memset((void *) scd, 0, sizeof(StreamCon));

    /* allocate memory buffer for this StreamCon, store pointer to it */
    scd->bufsize = bufsize;
    scd->buffer = malloc(bufsize);
    if (scd->buffer == NULL)
    {
	strcpy(szError, "malloc() failed");
	return -1;
    }

    /* success */
    return 0;
}

/************************************************************************
*									*
*  SCIO_open - open Stream Connection for network I/O			*
*									*
*  This fills in the fields of a StreamCon data structure and tries	*
*  to open the addresses returned from DNS serially (the way most code  *
*  today works, for comparison). 					*
*									*
*  For normal usage when it is not known which IP family the specified	*
*  server supports, specify af as AF_UNSPEC. Upon return, the af field  *
*  of the scd structure will contain AF_INET if it connected over IPv4	*
*  or AF_INET6 if it connected over IPv6. 
*									*
*    StreamCon	*scd		StreamCon structure			*
*    char	szNodename	Name of node to connect to		*
*    char	szService	Name of service to connect to 		*
*    int	af		Address family				*
*				AF_INET - IPv4				*
*				AF_INET6 - IPv6				*
*				AF_UNSPEC - Dual Stack			*
*    char	*szMode		I/O mode				*
*    char	*szError	returned error message			*
*									*
*  Returned value (int)							*
*									*
*    -1		error (message in szError)				*
*    0		success							*
*									*
************************************************************************/

int SCIO_open(StreamCon *scd, char *szNodename, char *szService, int af, 
	char *szMode, char *szError)
{
    int sd, rc;
    char szHost[256], szPort[16];

    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;
    struct addrinfo ai_hints, *ai_list, *ai;

    /* error if Stream already open */
    if(scd->sd != 0)
    {
	strcpy(szError, "StreamCon already open.");
	return -1;	/* error */
    }

    /* create hints for getaddrinfo for stream socket */
    memset(&ai_hints, 0, sizeof(ai_hints));
    ai_hints.ai_family = af;
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_protocol = IPPROTO_TCP;

    /* get list of addresses for nodename from DNS */
    rc = getaddrinfo(szNodename, szService, &ai_hints, &ai_list);
    if (rc != 0)
    {
	strcpy(szError, "getaddrinfo() failed: ");
	strcat(szError, gai_strerror(rc));
	return -1;
    }

    /* loop through returned ai_list */
    for (ai = ai_list; ai != NULL; ai = ai->ai_next)
    {
	/* display protocol specific formatted address */
	getnameinfo(ai->ai_addr, ai->ai_addrlen, szHost, sizeof(szHost),
	    szPort, sizeof(szPort), NI_NUMERICHOST | NI_NUMERICSERV);
#ifdef debugout
	printf("getnameinfo(): szHost=%s, szPort=%s\n", szHost, szPort);
#endif

	/* create socket */
	sd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (sd < 0)
	{
	   /* return error */
	    strcpy(szError, "socket() failed: ");
	    strcat(szError, strerror(errno));
	    return -1;	/* error: socket() failed */
	}
    
	/* try to connect with this entry in ai_list */
	rc = connect(sd, ai->ai_addr, ai->ai_addrlen);
	if (rc < 0)
	{
	    strcpy(szError, "connect() failed: ");
	    strcat(szError, strerror(errno));

	    /* go on to next entry in ai_list */
	    continue;
	}
#ifdef debugout
	printf("connect() succeeded\n");
#endif

	/* update scd */
	scd->sd = sd;			/* store socket descriptor */
	scd->bufptr = scd->buffer;	/* reset buffer pointer */
	scd->bufcount = 0;		/* reset buffer count to zero */
	scd->af = ai->ai_family;	/* keep track of address family */

	/* release memory from getaddrinfo() */
	freeaddrinfo(ai_list);

	return 0;
    }

    /* if we make it all the way through the list then */
    /* no returned addresses accepted connection */
    strcpy(szError, "unable to connect");

    /* release memory from getaddrinfo() */
    freeaddrinfo(ai_list);

    /* error exit */
    return -1;
}

/************************************************************************
*									*
*  SCIO_open_p - open Stream Connection for network I/O (parallel)	*
*									*
*  This fills in the fields of a StreamCon data structure and tries	*
*  to open all of the the addresses returned from DNS in parallel.	*
*  Only the first successful connection will be left in place, all	*
*  other connections will be aborted or disconnected.			*
*									*
*  For normal usage when it is not known which IP family the specified	*
*  server supports, specify af as AF_UNSPEC. Upon return, the af field  *
*  of the scd structure will contain AF_INET if it connected over IPv4	*
*  or AF_INET6 if it connected over IPv6. 
*									*
*    StreamCon	*scd		StreamCon structure			*
*    char	szNodename	Name of node to connect to		*
*    char	szService	Name of service to connect to 		*
*    int	af		Address family				*
*				AF_INET - IPv4				*
*				AF_INET6 - IPv6				*
*				AF_UNSPEC - Dual Stack			*
*    char	*szMode		I/O mode				*
*    int	timeout		overall timeout, any IP version (msec)	*
*    int	timeout6	IPv6 timeout (msec)			*
*    char	*szError	returned error message			*
*									*
*  Returned value (int)							*
*									*
*    -1		error (message in szError)				*
*    0		success							*
*									*
************************************************************************/

int SCIO_open_p(StreamCon *scd, char *szNodename, char *szService, int af, 
	char *szMode, int timeout, int timeout6, char *szError)
{
    int sd[20], sdx, flags, firstconn, v4count, v6count;
    int i, j, n, rc, msecsleep;
    char szHost[256], szPort[16];

    struct addrinfo ai_hints, *ai_list, *ai;
    struct sockaddr_storage peer;
    socklen_t peer_len;
    struct timespec pause, remain;

    msecsleep = 10;	/* number of msec to sleep before trying again */

    /* error if Stream already open */
    if(scd->sd != 0)
    {
	strcpy(szError, "StreamCon already open.");
	return -1;	/* error */
    }

    /* create hints for getaddrinfo for stream socket */
    memset(&ai_hints, 0, sizeof(ai_hints));
    ai_hints.ai_family = af;
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_protocol = IPPROTO_TCP;

    /* get list of addresses for nodename from DNS */
    rc = getaddrinfo(szNodename, szService, &ai_hints, &ai_list);
    if (rc != 0)
    {
	strcpy(szError, "getaddrinfo() failed: ");
	strcat(szError, gai_strerror(rc));
	return -1;
    }

    /* loop through returned ai_list, create array of sockets, each opened */
    v4count = v6count = 0;
    n = 0;
    for (ai = ai_list; ai != NULL; ai = ai->ai_next)
    {
	/* display protocol specific formatted address */
	getnameinfo(ai->ai_addr, ai->ai_addrlen, szHost, sizeof(szHost),
	    szPort, sizeof(szPort), NI_NUMERICHOST | NI_NUMERICSERV);
#ifdef debugout
	printf("getnameinfo(): szHost=%s, szPort=%s\n", szHost, szPort);
#endif

	/* if this record is AAAA, increment v4count, else incr. v6count */
	if (ai->ai_family == AF_INET)
	    v4count++;
	else
	    v6count++;

	/* create socket for this address */
	sd[n] = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (sd[n] < 0)
	{
	    /* fail */
	    strcpy(szError, "socket() failed, ");
	    strcat(szError, strerror(errno));
	    return -1;
	}
#ifdef debugout
	printf("created socket[%d]\n", n);
#endif
	
	/* make socket non-blocking (set O_NONBLOCK flag) */
	flags = fcntl(sd[n], F_GETFL, 0);
	fcntl(sd[n], F_SETFL, flags | O_NONBLOCK);

	/* do non-blocking connect with this socket */
	rc = connect(sd[n], ai->ai_addr, ai->ai_addrlen);
#ifdef debugout
	printf("non-blocking connect() complete\n");
#endif

	/* increment connection count, done if already MAXCONN */
	n++;
	if (n == 20)
	    break;
    }

    /* if no AAAA records retrieved, set timeout6 to zero (accept */
    /* IPv4 connections immediately */
    if (v6count == 0)
    {
#ifdef debugout
	printf("no AAAA records found, using IPv4 immediately\n");
#endif
	timeout6 = 0;
    }

    firstconn = -1;			/* no first connection so far */
    /* loop while waiting for connection */
    while (1)
    {
	/* go through list of sockets */
	for (i = 0, ai = ai_list; i < n; i++, ai = ai->ai_next)
	{
	    peer_len = sizeof(peer);
	    rc = getpeername(sd[i], (struct sockaddr *) &peer, &peer_len);
	    /* if error, no connection yet on this socket, keep trying */
	    if (rc < 0)
		continue;

	    /* no error - successful connection! */

	    /* check version of IP that connected */
	    if (ai->ai_family == AF_INET)
	    {
#ifdef debugout
		printf("connection successful on IPv4, socket[%d]\n", i);
#endif

		/* IPv4 connection, if this is first connection, note it */
		if (firstconn == -1)
		{
		    firstconn = 4;
		    sdx = sd[i];
		}
		break;
	    }
	    else
	    {
#ifdef debugout
		printf("connection successful on IPv6, socket[%d]\n", i);
#endif

		/* IPv6 connection, we're done */
		firstconn = 6;
		sdx = sd[i];
		break;
	    }
	}

	/* if connected over IPv6, we're done */
	if (firstconn == 6)
	{
#ifdef debugout
	    printf("we got a connection over IPv6 in time\n");
#endif
	    break;
	}

	/* sleep for another msecsleep msec */
	pause.tv_sec = 0;
	pause.tv_nsec = msecsleep * 1000000;
	nanosleep(&pause, &remain);

	/* subtract pause time from total timeout, if neg, give up */
	timeout = timeout - msecsleep;
	if (timeout <= 0)
	{
	    break;
	}

	/* if still positive, subtract pause time from timeout6 count */
	if (timeout6 > 0)
	    timeout6 = timeout6 - msecsleep;

	/* if given up on v6 and have v4 connection, done */
	if (timeout6 <= 0 && firstconn == 4)
	{
#ifdef debugout
	    printf("no connection over IPv6 in time, but got one on IPv4\n");
#endif
	    break;
	}
    }

    /* we either connected or timed out */

    /* done if we managed to connect */

    if (firstconn != -1)
    {
	/* update scd */
	scd->sd = sdx;			/* store socket descriptor */
	scd->bufptr = scd->buffer;	/* reset buffer pointer */
	scd->bufcount = 0;		/* reset buffer count to zero */
	scd->af = ai->ai_family;	/* keep track of address family */

	/* make winning socket blocking again */
	flags = fcntl(sdx, F_GETFL, 0);
	fcntl(sdx, F_SETFL, flags & (~ O_NONBLOCK));

	/* close all the sockets we didn't use */
	for (i = 0; i < n; i++)
	{
	    if (sd[i] == sdx)		/* don't close winning socket */
		continue;
	    if (sd[i] != 0)		/* if other socket open, close it */
	    {
#ifdef debugout
		printf("closing socket[%d]\n", i);
#endif
		close(sd[i]);
	    }
	}

	/* release ai_list allocation */
	freeaddrinfo(ai_list);

	/* return with winning socket in scd, open and ready to go */
	return 0;
    }

    /* timed out with no connection at all */
    /* close all the open sockets */
    for (i = 0; i < n; i++)
    {
	if (sd[i] != 0)
	    close(sd[i]);
    }
    strcpy(szError, "no connection");

    /* release ai_list allocation */
    freeaddrinfo(ai_list);

    return -1;
}

/************************************************************************
*									*
*  SCIO_putline - write one line of ASCII text to StreamCon		*
*									*
*  This routine allows writing one line of ASCII characters to the	*
*  stream connection specified in StreamCon data structure. It writes	*
*  characters until a zero byte is encountered. It does not add any	*
*  End of Line characters (LF or CR,LF). If you want these written,	*
*  for example, in SMTP protocol, include them before the zero byte.	*
*									*
*    StreamCon	*scd		StreamCon structure			*
*    char	*szOutbuf	line to write, term. by zero byte	*
*									*
*  Returned value (int)							*
*									*
*    -1		error (message in szError)				*
*    0		success							*
*									*
************************************************************************/

int SCIO_putline(StreamCon *scd, char *szOutbuf)
{
    int count, nwritten;

    count = strlen(szOutbuf);

    /* write it to stream connection */
    nwritten = write(scd->sd, szOutbuf, count);

    return nwritten;
}

/************************************************************************
*									*
*  SCIO_getline - read one line from Stream Connection (up to EOL)	*
*									*
*  This routine reads characters from the file specified in the		*
*  specified StreamCon data structure, using buffering. It will read	*
*  characters up until a LF or CR,LF character, but does not return	*
*  those characters. The returned characters will be terminated by a	*
*  zero byte. It uses SCIO_getch() to read individual characters from	*
*  the specified stream.						*
*									*
*    StreamCon *scd	pointer to StreamCon datastructure		*
*    char *inbuf	buffer to return line (term by '\0, no LF)	*
*    int maxlen		max number of chars to store (incl. '\0')	*
*									*
*  Return Value								*
*									*
*    -1			hard error					*
*    -2			EOF (other end closed stream)			*
*    > 0		normal read (number of bytes stored)		*
*									*
************************************************************************/

int SCIO_getline(StreamCon *scd, char *inbuf, int maxlen)
{
    int nstored;
    char ch, *inptr;

    /* reset input pointer to start of inbuf */
    inptr = inbuf;

    /* reset number of bytes stored */
    nstored = 0;

    /* loop until EOL, EOF, or error */
    while (1)
    {
	/* read one char from network */
	ch = SCIO_getch(scd);

	/* return now on error or EOF */
	if (ch == -1)
	    return -1;

	/* handle EOF */
	if (ch == -2)
	{
	    /* partial line, treat as EOL */
	    if (nstored > 0)
		break;	    

	    /* else, return EOF */
	    return -2;
	}

	/* normal read */

	/* ignore LF */
	if (ch == '\n')
	    continue;

	/* done if CR */
	if (ch == '\r')
	    break;

	/* if still room in inbuf, store char at end */
	if (nstored < maxlen-2)
	{
	    *inptr++ = ch;
	    nstored++;
	}
    }

    /* EOL, terminate inbuf with zero byte */
    *inptr = '\0';
    return nstored;
}

/************************************************************************
*									*
*  SCIO_getch - read one character from Stream Connection		*
*									*
*    StreamCon *scd	pointer to StreamCon datastructure		*
*									*
*  Return Value (char)							*
*									*
*    -1			hard error					*
*    -2			EOF (other end closed stream)			*
*    any other value	character read					*
*									*
************************************************************************/

char SCIO_getch(StreamCon *scd)
{
    int count;
    char ch;

    /* if buffer empty, try to fill it */
    if (scd->bufcount == 0)
    {
getch1:
	count = read(scd->sd, scd->buffer, scd->bufsize);
	if (count < 0)
	{
	    if (errno == EINTR)
		goto getch1;
	    /* else hard error */
	    return -1;
	}

	if (count == 0)
	    return -2;		/* EOF, return -2 */
	
	/* normal read, set buffer count and pointer */
	scd->bufcount = count;
	scd->bufptr = scd->buffer;
    }

    /* get next character from buffer */
    ch = *(scd->bufptr);
    scd->bufcount--;
    scd->bufptr++;

    return ch;		/* normal exit, return character read */
}

/************************************************************************
*									*
*  SCIO_close - close StreamCon connection				*
*									*
*  Closes StreamCon connection, but does not deallocate buffer memory.	*
*  If the StreamCon data structure will not be reused, be sure to also	*
*  call SCIO_free.							*
*									*
*    StreamCon *scd	pointer to StreamCon datastructure		*
*									*
*  Return Value (char)							*
*									*
*    -1			hard error					*
*    0			success						*
*									*
************************************************************************/

int SCIO_close(StreamCon *scd, char *szError)
{
    int rc;

    /* error if StreamCon not currently open */
    if(scd->sd <= 0)
    {
	strcpy(szError, "Stream not currently open");
	return -1;	/* error */
    }

    /* try to close socket */
    rc = close(scd->sd);

    /* handle error return */
    if (rc < 0)
    {
	strcpy(szError, "close() failed: ");
	strcat(szError, strerror(errno));
	return -1;
    }

    /* close successful, clear sd field in StreamCon structure */
    scd->sd = 0;

    /* normal exit */
    return 0;
}

/************************************************************************
*									*
*  SCIO_free - free input buffer allocated by SCIO_init			*
*									*
*  Release memory allocated for a Stream connection I/O buffer.		*
*									*
*    StreamCon *scd	pointer to StreamCon datastructure		*
*									*
*  Return Value (char)							*
*									*
*    0			success						*
*									*
************************************************************************/

int SCIO_free(StreamCon *scd)
{
    /* free memory buffer for this StreamCon */
    free(scd->buffer);

    /* clear relevant StreamCon structure fields */
    scd->buffer = NULL;
    scd->bufsize = 0;

    /* success */
    return 0;
}


