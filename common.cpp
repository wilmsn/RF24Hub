#include "common.h"

char * trim (char * s) {
  /* Initialize start, end pointers */
  char *s1 = s, *s2 = &s[strlen (s) - 1];
  /* Trim and delimit right side */
  while ( (isspace (*s2)) && (s2 >= s1) )
    s2--;
  *(s2+1) = '\0';
  /* Trim left side */
  while ( (isspace (*s1)) && (s1 < s2) )
    s1++;
  /* Copy finished string */
  strcpy (s, s1);
  return s;
}

char * log_ts(char * buf) {
    char temp[20];
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int msec=tv.tv_usec / 1000;
    sprintf(buf,"[%s.%s%s%u] ",str_ts(temp,1),msec<100? "0":"",msec<10? "0":"",msec); 
    return buf;
}

char * str_ts(char * buf, uint8_t form) {
    time_t now = time(0);
	tm *ltm = localtime(&now);
    char temp[20];
    switch ( form ) {
        case 1: {
            sprintf(temp,"%d.%s%d.%s%d %s%d:%s%d:%s%d",ltm->tm_year + 1900, 
                    ltm->tm_mon<9? "0":"", ltm->tm_mon+1, ltm->tm_mday<10? "0":"", ltm->tm_mday,
                    ltm->tm_hour<10? " ":"", ltm->tm_hour, ltm->tm_min<10? "0":"", ltm->tm_min,
                    ltm->tm_sec<10? "0":"", ltm->tm_sec);
        }
        break;
        case 2: {
            sprintf(temp,"%s%d.%s%d.%d %s%d:%s%d:%s%d", ltm->tm_mday<10? " ":"", ltm->tm_mday, 
                    ltm->tm_mon<10? "0":"", ltm->tm_mon, ltm->tm_year + 1900,
                    ltm->tm_hour<10? " ":"", ltm->tm_hour, ltm->tm_min<10? "0":"", ltm->tm_min,
                    ltm->tm_sec<10? "0":"", ltm->tm_sec);
        }
        break;
    }        
    strcpy (buf, temp);
    return buf;
}

uint64_t mymillis(void) {
	struct timeval tv;
	uint64_t timebuf;
	gettimeofday(&tv, NULL);
	timebuf = (((tv.tv_sec & 0x000FFFFFFFFFFFFF) * 1000) + (tv.tv_usec / 1000));
	return timebuf;
}

uint16_t decodeVerbose(uint16_t oldLevel, char* verboseSet) {
    uint16_t retval = 0;
    char cmp_addVBtn[]="+telnet",
         cmp_rmVBtn[]="-telnet",
         cmp_addVBrf24[]="+rf24",
         cmp_rmVBrf24[]="-rf24",
         cmp_addVBob[]="+obuffer",
         cmp_rmVBob[]="-obuffer",
         cmp_addVBorder[]="+order",
         cmp_rmVBorder[]="-order",
         cmp_addVBsql[]="+sql",
         cmp_rmVBsql[]="-sql",
         cmp_addVBpnt[]="+pointer",
         cmp_rmVBpnt[]="-pointer";
    if (strcmp(verboseSet,cmp_addVBtn) == 0) {
        retval = oldLevel | VERBOSETELNET;
    }
    if (strcmp(verboseSet,cmp_rmVBtn) == 0) {
        retval = oldLevel ^ VERBOSETELNET;
    }
    if (strcmp(verboseSet,cmp_addVBrf24) == 0) {
        retval = oldLevel | VERBOSERF24;
    }
    if (strcmp(verboseSet,cmp_rmVBrf24) == 0) {
        retval = oldLevel ^ VERBOSERF24;
    }    
    if (strcmp(verboseSet,cmp_addVBsql) == 0) {
        retval = oldLevel | VERBOSESQL;
    }
    if (strcmp(verboseSet,cmp_rmVBsql) == 0) {
        retval = oldLevel ^ VERBOSESQL;
    }    
    if (strcmp(verboseSet,cmp_addVBpnt) == 0) {
        retval = oldLevel | VERBOSEPOINTER;
    }
    if (strcmp(verboseSet,cmp_rmVBpnt) == 0) {
        retval = oldLevel ^ VERBOSEPOINTER;
    }    
    if (strcmp(verboseSet,cmp_addVBob) == 0) {
        retval = oldLevel | VERBOSEOBUFFER;
    }
    if (strcmp(verboseSet,cmp_rmVBob) == 0) {
        retval = oldLevel ^ VERBOSEOBUFFER;
    }    
    if (strcmp(verboseSet,cmp_addVBorder) == 0) {
        retval = oldLevel | VERBOSEORDER;
    }
    if (strcmp(verboseSet,cmp_rmVBorder) == 0) {
        retval = oldLevel ^ VERBOSEORDER;
    }    
    return retval;
}


char* alloc_str(uint16_t verboselevel, const char* msgTxt, size_t size) {
    if (verboselevel & VERBOSEPOINTER) {
        char buf[] = TSBUFFERSTRING;
        printf("%sMalloc %s P:", log_ts(buf), msgTxt);
    }    
    char *retval =  (char*) malloc (size);
    memset(retval, 0, size);
    if (verboselevel & VERBOSEPOINTER) {
        char buf[] = TSBUFFERSTRING;
        printf("<%p> OK\n", retval);
    }    
    return retval;
}

void free_str(uint16_t verboselevel, const char* msgTxt, char* str) {
    if (verboselevel & VERBOSEPOINTER) {
        char buf[] = TSBUFFERSTRING;
        printf("%sFree %s P:<%p>", log_ts(buf), msgTxt, str);
    }    
    free(str);
    if (verboselevel & VERBOSEPOINTER) {
        char buf[] = TSBUFFERSTRING;
        printf(" OK\n");
    }    
}
