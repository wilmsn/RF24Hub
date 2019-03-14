#include "rf24hub_common.h"
#include "config.h"
#include "telnet.h"
#include "DB-mariaDB.h"
#include "sensorBuffer.h"
#include "orderBuffer.h"
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>
#include <signal.h>

using namespace std; 

CONFIG cfg(PRGNAME, PRGVERSION);
DB db(&cfg);
SENSORBUFFER sensorbuffer;
ORDERBUFFER orderbuffer;
int tn_in_socket, new_tn_in_socket;
socklen_t addrlen;
//char *buffer =  (char*) malloc (BUF);
long save_fd;
const int y = 1;
//char client_message[30];
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
    string debug;
    cfg.processParams(argc, argv);
	// check if started as root
	if ( getuid()!=0 ) {
		cout << "rf24hubd has to be startet as user root" << endl; 
        exit(1);
    }
	cout << "Startup Parameters:" << endl; 
    cfg.printConfig();
    // check for PID file, if exists terminate else create it
    if ( cfg.checkPidFileSet() ) return 1;
	
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
                cout << "Starting rf24hubd as daemon..." << endl;
                // and exit
                exit (0);
            } else {
                // nagativ is an error
                cfg.removePidFile();
                exit (1);
            }
        } else {
            cout << "Error: Logfile is needed if runs as deamon ... exiting" << endl;
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
			debug = "Binding Socket on Port";
            debug += cfg.parms.incoming_port;
            debug += " OK";
		}
		listen (tn_in_socket, 5);
		addrlen = sizeof (struct sockaddr_in);
		save_fd = fcntl( tn_in_socket, F_GETFL );
		save_fd |= O_NONBLOCK;
		fcntl( tn_in_socket, F_SETFL, save_fd );
    }
    if ( ! db.init(cfg.parms.db_hostname, cfg.parms.db_port, cfg.parms.db_schema, cfg.parms.db_username, cfg.parms.db_password) ) {
        cfg.logmsg(VERBOSECRITICAL,"Error connecting to Database");
        exit(1);
    }
    db.initSystem();
    db.fillSensorBuffer(&sensorbuffer);
    sensorbuffer.listSensor();
    int i=1;
    while(1) {
        /* Handling of incoming messages */
        new_tn_in_socket = accept ( tn_in_socket, (struct sockaddr *) &address, &addrlen );
		if (new_tn_in_socket > 0) {
            //receive_tn_in(new_tn_in_socket, &address);
            thread t2(receive_tn_in, new_tn_in_socket, &address);
            t2.detach();
            //close (new_tn_in_socket);
        }

        
        usleep(100000);
    
            
       // cout << "Test ..." << i << endl;
        i++;
        if (i>1000) i=1;
        
        //break;
    } // while(1)
    cout << "Exit programm..." << endl;
    exit_system();
    cfg.removePidFile();
	return 0;
}

