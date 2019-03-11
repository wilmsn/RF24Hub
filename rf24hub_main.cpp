#include "rf24hub_common.h"
#include "config.h"
#include "telnet.h"
//#include "DB-mariaDB.h"
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>
#include <signal.h>

using namespace std; 

CONFIG cfg(PRGNAME, PRGVERSION);
int tn_in_socket, new_tn_in_socket;
socklen_t addrlen;
char *buffer =  (char*) malloc (BUF);
long save_fd;
const int y = 1;
char client_message[30];
struct sockaddr_in address;


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

    // init SIGTERM and SIGINT handling
    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);
    
    if (cfg.start_daemon) {
        // make sure that we have a logfile
        if ( cfg.logfile_mode ) {
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
        } else {
            std::cout << "Error: Logfile is needed if runs as deamon ... exiting" << std::endl;
            cfg.removePidFile();
            exit(1);
        }
    }
    if (cfg.in_port_set) {
   		if ( (tn_in_socket=socket( AF_INET, SOCK_STREAM, 0)) > 0) {
			cfg.logmsg(VERBOSESTARTUP, "Socket für eingehende Messages angelegt");
		}
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons (cfg.parms.incoming_port);
		setsockopt( tn_in_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int) );
		if (bind( tn_in_socket, (struct sockaddr *) &address, sizeof (address)) == 0 ) {
			std::string debug = "Binding Socket on Port";
            debug += cfg.parms.incoming_port;
            debug += " OK";
		}
		listen (tn_in_socket, 5);
		addrlen = sizeof (struct sockaddr_in);
		save_fd = fcntl( tn_in_socket, F_GETFL );
		save_fd |= O_NONBLOCK;
		fcntl( tn_in_socket, F_SETFL, save_fd );
    }
    int i=1;
    while(1) {
        /* Handling of incoming messages */
        new_tn_in_socket = accept ( tn_in_socket, (struct sockaddr *) &address, &addrlen );
		if (new_tn_in_socket > 0) {
            //receive_tn_in(new_tn_in_socket, &address);
            std::thread t2(receive_tn_in, new_tn_in_socket, &address);
            t2.detach();
            //close (new_tn_in_socket);
        }

        
        usleep(10000);
    
            
        std::cout << "Test ..." << i << std::endl;
        i++;
        if (i>1000) i=1;
        
        //break;
    } // while(1)
    std::cout << "Exit programm..." << std::endl;
    exit_system();
    cfg.removePidFile();
	return 0;
}

