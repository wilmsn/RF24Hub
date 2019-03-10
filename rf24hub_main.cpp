#include "rf24hub_common.h"
#include "config.h"
//#include "telnet.h"
#include "logmsg.h"
#include "DB-mariaDB.h"
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

CONFIG cfg(PRGNAME, PRGVERSION);


uint64_t mymillis(void) {
	struct timeval tv;
	uint64_t timebuf;
	gettimeofday(&tv, NULL);
//	timebuf = ((tv.tv_sec & 0x000FFFFF) * 1000 + (tv.tv_usec / 1000)) - start_time;
	return timebuf;
}



void exit_system(void) {
	cfg.logmsg(VERBOSESTARTUP, "SIGTERM: Cleanup system ... saving *_im tables ...");
    // Save data from sensordata_im and sensor_im to persistant tables
/*	sprintf (sql_stmt, "update sensor a set value = ( select value from sensor_im where sensor_id = a.sensor_id ), utime = ( select utime from sensor_im where sensor_id = a.sensor_id )");
	logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "insert into sensordata(sensor_id, utime, value) select sensor_id, utime, value from sensordata_im where (sensor_id,utime) not in (select sensor_id, utime from sensordata)");
	logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
*/	
}

void sighandler(int signal) {
    exit_system(); 
	cfg.logmsg(VERBOSESTARTUP, "SIGTERM: Shutting down ... ");
    cfg.removePidFile();
    exit (0);
}

using namespace std; 


int main(int argc, char* argv[]) {
    pid_t pid;
    cfg.processParams(argc, argv);
	// check if started as root
	if ( getuid()!=0 ) {
		std::cout << "rf24hubd has to be startet as user root" << std::endl; 
        exit(1);
    }
	std::cout << "Startup Parameters:" << std::endl; 
    cfg.printConfig();
    // check for PID file, if exists terminate else create it
    if ( ! cfg.checkPidFileSet() ) return 1;
	
    // starts logging
	if ( getuid()==0 ) {
       cfg.setPidFile();
    }
    
    if (cfg.startAsDeamon()) {
        // make sure that we have a logfile
        if ( ! cfg.checkLogFileSet() ) {
            std::cout << "Error: Logfile is needed if runs as deamon ... exiting" << std::endl;
            cfg.removePidFile();
            exit(1);
        } else {
            // starts rf24hubd as a deamon
            // no messages to console!
            pid = fork ();
            if (pid == 0) {
                // Child prozess
                chdir ("/");
                umask (0);
                cfg.logmsg(VERBOSESTARTUP, "Starting up ....");
            } else if (pid > 0) {
                // Parentprozess -> exit and return to shell
                // write a message to the console
                std::cout << "Starting rf24hubd as daemon..." << std::endl;
                // and exit
                exit (0);
            } else {
                // nagativ is an error
                cfg.removePidFile();
                exit (1);
            }
        }
    }
    while(1) {
        /* Handling of incoming messages */

        
        sleep(3);

        
        break;
    } // while(1)
    std::cout << "Exit programm..." << std::endl;
    exit_system();
    cfg.removePidFile();
	return 0;
}

