/************************************************************************
*									*
*  SCIO.h - Stream Connection Input/Output library (header)		*
*									*
*	Author:		Lawrence E. Hughes				*
*	Version:	1.0.1 - 24 April 2011				*
*	Platform(s):	Tested on FreeBSD 8.2, Ubuntu 10.10		*
*	Rights:		Copyright (c) 2011, Lawrence E. Hughes		*
*			All Rights Reserved, Worldwide			* 
*			Released under Simplified BSD license		*
*			www.opensource.org/licenses/bsd-license.php	*
*									*
*  See corresponding file SCIO.c for documentation on functions.	*
*									*
************************************************************************/

/* DEFINITIONS */

#define SFIO_read	0x01
#define SFIO_write	0x02

/* TYPEDEFS */

/* StreamCon - data structure for buffered I/O Stream Connection */

typedef struct {
    int			sd;		/* socket descriptor */
    int			mode;		/* mode: SF_read, SF_write */
    int			af;		/* address family: AF_INET, etc. */
    unsigned char	*buffer;	/* file I/O buffer */
    int			bufsize;	/* buffer size in bytes */
    int			bufcount;	/* num of bytes available in buffer */
    unsigned char	*bufptr;	/* next available byte in buffer */
} StreamCon;

/* FUNCTION PROTOTYPES */

int SCIO_init(StreamCon *scd, int bufsize, char *szError);
int SCIO_open(StreamCon *scd, char *szNodename, char *szService, int af, 
	char *szMode, char *szError);
int SCIO_open_p(StreamCon *scd, char *szNodename, char *szService, int af, 
	char *szMode, int timeout, int timeout6, char *szError);

int SCIO_putline(StreamCon *scd, char *outbuf);
int SCIO_putch(StreamCon *scd, char ch);

int SCIO_getline(StreamCon *scd, char *inbuf, int maxlen);
char SCIO_getch(StreamCon *scd);

int SCIO_close(StreamCon *scd, char *szError);
int SCIO_free(StreamCon *scd);


