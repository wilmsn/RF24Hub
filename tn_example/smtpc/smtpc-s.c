/************************************************************************
*									*
*  smtpc-s - dual stack client demo (using serial connect)		*
*									*
*  Author:	Lawrence E. Hughes					*
*  History:	v1.0 - 24 April 2011					*
*									*
*  Platform:	Tested on FreeBSD 8.2, Ubuntu 10.10			*
*									*
*  Copyright (c) 2011, Lawrence E. Hughes				*
*  All Rights Reserved Worldwide					*
*  Released under Simplified BSD license				*
*  www.opensource.org/licenses/bsd-license.php				*
*									*
*  This code is to illustrate use of the SCIO library, for either	*
*  serial or parallel connection to a dual stack server (in this case	*
*  to an SMTP server). Once it connects, it does a very simple SMTP	*
*  session then disconnects.						*
*									*
*  For serial connection, use SCIO_open(). For parallel connection, use	*
*  SCIO_open_p(). Note that the two routines have different calling	*
*  parameters (described in SCIO.c).					*
*									*
*  You can build this by putting smtp_demo.c, SCIO.c and SCIO.h all in	*
*  one directory, then issuing the following command:			*
*									*
*  	$ gcc -o smtpc smtpc.c SCIO.c					*
*									*
*  This will produce an executable on file smtpc			*
*									*
*  You can run it by specifying the name of a mail server as the first	*
*  parameter. For example:						*
*									*
*	$ smtpc www.v6mail.net						*
*									*
************************************************************************/

/***********************
* HEADER FILE INCLUDES *
***********************/

#include "SCIO.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <errno.h>

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


/************************
* FUNCTION DECLARATIONS *
************************/

int chopcrlf(char szString[]);

/***********************
* FUNCTION DEFINIITONS *
***********************/

/************************************************************************
*									*
*  main - main program of dual stack client demo			*
*									*
************************************************************************/

int main(int argc, char *argv[])
{
    int sd4, sd6;	// listen sockets for both v4 and v6
    int csd4, csd6;	// client sockets for both v4 and v6
    int rc, rv, opts, i, maxsd;
    int yes=1;

    char szServer[1024], szError[1024], szAddrFamily[8];
    char szInbuf[1024], szOutbuf[1024];

    StreamCon scd;

    /* get name of mail server (ideally dual stack) from command line */
    strcpy(szServer, argv[1]);

    fprintf(stderr, "Connecting to SMTP server %s\n", szServer);

    /* initialize SCIO connection */
    rc = SCIO_init(&scd, 8192, szError);
    if (rc < 0)
    {
	printf("SCIO_init() failed\n");
	exit(1);
    }

    /* open connection, parallel mode, allowing up to 15 seconds before */
    /* giving up. If after 3 seconds, no IPv6 connection is obtained, */
    /* fall back to IPv4, which is probably already connected. */
    rc = SCIO_open(&scd, szServer, "smtp", AF_UNSPEC, "r", szError);
    if (rc < 0)
    {
	printf("SCIO_open() failed, %s\n", szError);
	exit(1);
    }

    /* give feedback as to which IP family connection was made over */
    if (scd.af == AF_INET)
	strcpy(szAddrFamily, "IPv4");
    else
	strcpy(szAddrFamily, "IPv6");

    printf("Connected over %s\n", szAddrFamily);

    /* get greeting from server */
    rc = SCIO_getline(&scd, szInbuf, sizeof(szInbuf));
    if (rc < 0)
    {
	printf("SCIO_getline() failed\n");
	exit(1);
    }

    /* display it, as coming from server */
    printf("S: %s\n", szInbuf);

    /* send EHLO command */
    strcpy(szOutbuf, "EHLO x.com\r\n");
    rc = SCIO_putline(&scd, szOutbuf);
    if (rc < 0)
    {
	printf("SCIO_putline() failed\n");
	exit(1);
    }

    /* display it, as coming from client */
    chopcrlf(szOutbuf);
    printf("C: %s\n", szOutbuf);

    /* accept capabilities from ESMTP server, or just OK from SMTP */
    while (1)
    {
	/* get server capabilities */
	rc = SCIO_getline(&scd, szInbuf, sizeof(szInbuf));
	if (rc < 0)
	{
	    printf("SCIO_open_p() failed\n");
	    exit(1);
	}

	/* display response */
	printf("S: %s\n", szInbuf);

	/* check for non-ESMTP server */
	if (strncmp(szInbuf, "250", 3) != 0)    
	{
	    printf("Server doesn't support ESMTP\n");
	    break;
	}

	/* if char after 250 not '-', done */
	if (szInbuf[3] != '-')
	    break;
    }

    /* send QUIT command */
    strcpy(szOutbuf, "QUIT\r\n");
    rc = SCIO_putline(&scd, szOutbuf);
    if (rc < 0)
    {
	printf("SCIO_putline() failed\n");
	exit(1);
    }

    /* show it as coming from client */
    chopcrlf(szOutbuf);
    printf("C: %s\n", szOutbuf);

    /* get response from QUIT command */
    rc = SCIO_getline(&scd, szInbuf, sizeof(szInbuf));
    if (rc < 0)
    {
	printf("SCIO_getline() failed\n");
	exit(1);
    }

    /* display it as coming from server */
    printf("S: %s\n", szInbuf);

    /* close connection */
    rc = SCIO_close(&scd, szError);
    if (rc < 0)
    {
	printf("SCIO_close() failed\n");
	exit(1);
    }

    /* free allocated memory for input buffer */
    SCIO_free(&scd);

    /* normal exit */
    exit(0);
}

int chopcrlf(char szString[])
{
    int i, j, len;

    len = strlen(szString);
    j = 0;
    for (i = 0; i < len; i++)
    {
	if (szString[i] == '\r' || szString[i] == '\n')
	    continue;
	szString[j++] = szString[i];
    }
    szString[j] = '\0';
}

