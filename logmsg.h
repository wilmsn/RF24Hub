#ifndef RF24HUB_LOGMSG_H   
#define RF24HUB_LOGMSG_H

#include "rf24hub_common.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>

class LOGMSG {
    
private:

    bool log2console = false;
    bool log2file = false;
    std::string logfileName;
    int verboselevel=2;

public:
    LOGMSG();
    ~LOGMSG();
    void setVerboseLevel(int);
    int getVerboseLevel(void);
    void setLog2File(std::string);
    void unsetLog2File(void);
    void setLog2Console(void);
    void unsetLog2Console(void);
//    int setLog2logfile(void);
    void logmsg(int, std::string);

};

#endif //RF24HUB_LOGMSG_H
