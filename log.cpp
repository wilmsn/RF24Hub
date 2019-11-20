#include "log.h"

void Logger::logmsg(int mesgloglevel, char *mymsg){
    if (mesgloglevel <= verboselevel) {
		time_t now = time(0);
		tm *ltm = localtime(&now);
		sprintf (buf, "%d", ltm->tm_year + 1900 );
		if ( ltm->tm_mon + 1 < 10) sprintf(buf,"%s.0%d",buf,ltm->tm_mon + 1); else sprintf(buf,"%s.%d",buf,ltm->tm_mon + 1);
		if ( ltm->tm_mday < 10) sprintf(buf,"%s.0%d",buf,ltm->tm_mday); else sprintf(buf,"%s.%d",buf,ltm->tm_mday);
		if ( ltm->tm_hour < 10) sprintf(buf,"%s  %d",buf,ltm->tm_hour); else sprintf(buf,"%s %d",buf,ltm->tm_hour);
		if ( ltm->tm_min < 10) sprintf(buf,"%s:0%d",buf,ltm->tm_min); else sprintf(buf,"%s:%d",buf,ltm->tm_min);
		if ( ltm->tm_sec < 10) sprintf(buf,"%s:0%d",buf,ltm->tm_sec); else sprintf(buf,"%s:%d",buf,ltm->tm_sec);
        struct timeval tv;
        gettimeofday(&tv, NULL);
        unsigned int msec=tv.tv_usec / 1000;
        if (msec < 10) sprintf(buf,"%s.00%u",buf,msec);
        else if (msec < 100) sprintf(buf,"%s.0%u",buf,msec);
        else sprintf(buf,"%s.%u",buf,msec); 
        if ( logmode == logfile ) {
			logfile_ptr = fopen (logfilename,"a");
			if ( logfile_ptr != NULL ) {
				fprintf (logfile_ptr, "[%s] %s \n", buf, mymsg );
				fclose (logfile_ptr);
			}
        } else {  // logmode == interactive 
			fprintf(stdout, "[%s] %s\n", buf, mymsg); 
		}
//	} else { // log via systemlog
//		if (mesgloglevel <= verboselevel) {
//			openlog ( "sensorhubd", LOG_PID | LOG_CONS| LOG_NDELAY, LOG_LOCAL0 );
//			syslog( LOG_NOTICE, "%s\n", mymsg);
//			closelog();
//		}
	}
}

void Logger::set_logfile(char* _logfilename) {
    logfilename = _logfilename;
}

void Logger::set_logmode(char _logmode) {
    if (_logmode == 'i' ) {
        logmode=interactive;
    }
    if (_logmode == 'l' ) {
        logmode=logfile;
    }
    if (_logmode == 's' ) {
        logmode=systemlog;
    }
}

Logger::Logger(void) {
    
}
