#include "config.h"
#include "telnet.h"

using namespace std; 

string debug;

CONFIG cfg(PRGNAME, PRGVERSION);

int main(int argc, char* argv[]) {
    int my_tn = 0;
    int my_new_tn = 0;
    struct sockaddr_in address;
    socklen_t addrlen;
    addrlen = sizeof (struct sockaddr_in);
    cfg.processParams(argc, argv);
	// check if started as root
	cout << endl << "Startup Parameters:" << endl; 
    cfg.printConfig();
    // starts logging
	if ( getuid()==0 ) {
       cfg.setPidFile();
    }
    if (cfg.telnetPortSet) {
        cout << "Öffne Telnet Port" << endl;
        openTelnetSocket(cfg.parms.telnetPort.c_str(), &address, &my_tn );
        cout << "TN Port offen " << my_tn << endl;
        while(1) {
            if (my_tn > 0) {
                my_new_tn = accept ( my_tn, (struct sockaddr *) &address, &addrlen );
                if (my_new_tn > 0 ) {
                    cout << "my_new_tn: " << my_new_tn << endl;
                    thread t2(receiveTelnetMessage, my_new_tn, &address);
                    t2.detach();
                  //  close (my_new_tn);
                }
            }
            usleep(200000);
        }
    }
    
    cout << "Ausgabe der Usage:" << endl;
    cfg.usage();
	if ( getuid()==0 ) {
        cfg.removePidFile();
    }
    return 0;
}

