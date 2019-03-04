#include "telnet.h"
#include "config.h"
#include <fcntl.h>
#include <unistd.h>
#include <thread>


/* vars for telnet socket handling */
int tn_in_socket, new_tn_in_socket;
socklen_t addrlen;
char *buffer =  (char*) malloc (BUF);
struct sockaddr_in address;
long save_fd;
const int y = 1;
bool wait4message = false;
char debug[DEBUGSTRINGSIZE];
char client_message[30];
struct config_parameters parms;
bool tn_port_set = false;
bool tn_host_set = false;
bool in_port_set = false;
bool start_daemon = false;
logmode_t logmode;
int verboselevel = 2;
FILE * logfile_ptr;
FILE * pidfile_ptr;

char config_file[PARAM_MAXLEN_CONFIGFILE];



//tn_active = true;
//tn_in_socket=0;

int main() {
    /* open incoming port for messages */
		if ( (tn_in_socket=socket( AF_INET, SOCK_STREAM, 0)) > 0) {
			printf ("%s", "Socket für eingehende Messages angelegt");
		}
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons (7001);
		setsockopt( tn_in_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int) );
		if (bind( tn_in_socket, (struct sockaddr *) &address, sizeof (address)) == 0 ) {
			printf ("Binding Socket on Port %d OK", 7001);
		}
		listen (tn_in_socket, 5);
		addrlen = sizeof (struct sockaddr_in);
		save_fd = fcntl( tn_in_socket, F_GETFL );
		save_fd |= O_NONBLOCK;
		fcntl( tn_in_socket, F_SETFL, save_fd );
    sleep(2);

    	// Main Loop
    while(1) { 
        char debug[40];
        new_tn_in_socket = accept ( tn_in_socket, (struct sockaddr *) &address, &addrlen );
		if (new_tn_in_socket > 0) {
            //receive_tn_in(new_tn_in_socket, &address);
            std::thread t2(receive_tn_in, new_tn_in_socket, &address);
            t2.detach();
            //close (new_tn_in_socket);
        }
        sprintf(debug,"%s",".");
        logmsg(1,debug);
        usleep(20000);
        
        
    
    }   
} 
