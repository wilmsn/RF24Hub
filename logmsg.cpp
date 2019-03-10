#include "logmsg.h"

LOGMSG::LOGMSG() {
    
}

LOGMSG::~LOGMSG() {
    LOGMSG::logmsg(1,"Logfile closed");
}

void LOGMSG::setLog2Console(void) {
    LOGMSG::log2console = true;
}

void LOGMSG::unsetLog2Console(void) {
    LOGMSG::log2console = false;
}

void LOGMSG::setLog2File(std::string logfile_name) {
    LOGMSG::logfileName = logfile_name;
    LOGMSG::logmsg(1,"Logfile opened");
    LOGMSG::log2file = true;
}

void LOGMSG::unsetLog2File(void) {
    LOGMSG::log2file = false;
}

void LOGMSG::setVerboseLevel(int verboselevel) {
    LOGMSG::verboselevel = verboselevel;
}

int LOGMSG::getVerboseLevel(void) {
    return LOGMSG::verboselevel;
}

void LOGMSG::logmsg(int mesgloglevel, std::string mymsg){
	if (mesgloglevel <= LOGMSG::verboselevel) {
        if ( LOGMSG::log2file ) {
            std::string line;
            char timestr[20];
			char m0[2], d0[2], mi0[2], s0[2];
            m0[1]='\0'; d0[1]='\0'; mi0[1]='\0'; s0[1]='\0';
            time_t now = time(0);
			tm *ltm = localtime(&now);
            m0[0] = (ltm->tm_mon < 10) ? '0' : '\0';
            d0[0] = (ltm->tm_mday < 10) ? '0' : '\0';
            mi0[0] = (ltm->tm_min < 10) ? '0' : '\0';
            s0[0] = (ltm->tm_sec < 10) ? '0' : '\0';
            sprintf(timestr, "%d.%s%d.%s%d %d:%s%d:%s%d",ltm->tm_year + 1900,m0,ltm->tm_mon+1,d0,ltm->tm_mday,ltm->tm_hour, mi0, ltm->tm_min, s0, ltm->tm_sec);
            line = "[";
            line += timestr;
            line += "] ";
            line += mymsg;
            std::ofstream out(LOGMSG::logfileName.c_str(), std::ios_base::app);
            out << line << std::endl;
            out.close();
        }	
        if ( LOGMSG::log2console ) {  // logmode == interactive 
			std::cout << mymsg << std::endl; 
		}
    }
}

