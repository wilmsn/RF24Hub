#ifndef CFG_H   
#define CFG_H

#include <stdio.h>
#include <stdlib.h>
#include <string> // for string class 
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>
#include "config.h"
#include "common.h"

using namespace std; 

/* 
 * Grundsätzliches zum Ablauf einer Initialisierung:
 * * Übergebene Parameter überschreiben Einträge im Config File
 * * In den Config-Variablen werden die letztgültigen Werte abgelegt
 *
 * Folgende Parameter des Config Files können durch 
 * übergebene Parameter überschrieben werden:
 * 1. Logfile
 * 2. Verbosemode
 * 
 */

extern uint16_t verboselevel;

class Cfg {
    
private:

FILE * pidfile_ptr;
FILE * logfile_ptr;
// The filename incl. path of the configfile
string configFile;
// the programm name goes here
string prgName;
// the programm version goes here
string prgVersion;
    
public:
	
// Hier die verwendeten Variablen:
/**********************************************
 * Variablen für den Gateway
 **********************************************/
/**
 * Logfilename für rf24gwd
 */ 
string gwLogFileName;
/**
 * Pidfilename für rf24gwd
 */
string gwPidFileName;
/**
 * Speichert den eindeutigen Servernamen inkl. Domaine für rf24hubd
 */ 
string gwHubHostName;
/**
 * Eingehender UDP port vom rf24gwd
 * @note Zwischen rf24hubd und rf24gwd werden Anweisungen mittels UDP ausgetauscht.
 */
string gwUdpPortNo;
/**
 * Eingehender TCP Port vom rf24gwd
 * @note Über den TCP Port können Steuerbefehle an den rf24gwd gesendet werden
 * @code $>telnet server.domain <tcp-port>
 * 
 *  rf24gwd> set verbose +rf24 @endcode
 */
string gwTcpPortNo;
/**
 * gwTcpPortSet: true wenn ein eingehender TCP Port gesetzt ist, sonst flase
 */
bool gwTcpPortSet;
/**
 * gwUdpPortSet: True wenn ein eingehender UDP Port gesetzt ist, sonst flase
 */
bool gwUdpPortSet;
/**
 * Die eindeutige Nummer(ID) für diesem rf24gwd
 */
string gwNo;
/**********************************************
 * Variablen für den Hub
 **********************************************/
/**
 * Eingehender UDP port vom rf24hubd
 * @note Zwischen rf24hubd und rf24gwd werden Anweisungen mittels UDP ausgetauscht.
 */
string hubUdpPortNo;
/**
 * Logfilename für rf24hubd
 */ 
string hubLogFileName;
/**
 * Pidfilename für rf24hubd
 */ 
string hubPidFileName;
/**
 * Eingehender TCP Port vom rf24hubd
 * @note Über den TCP Port können Steuerbefehle an den rf24hubd gesendet werden. 
 * Diesen Port netzt auch FHEM um seine Steuerbefehle an den Hub zu senden. 
 * @code $>telnet server.domain <tcp-port>
 * 
 *  rf24hubd> set verbose +order @endcode
 */
string hubTcpPortNo;
/**
 * gwTcpPortSet: True wenn ein eingehender TCP Port gesetzt ist, sonst flase
 */
bool hubTcpPortSet;
/**
 * gwUdpPortSet: True wenn ein eingehender UDP Port gesetzt ist, sonst flase
 */
bool hubUdpPortSet;
/**********************************************
 * Database
 **********************************************/
/**
 * Hostname für den Datenbankserver
 */
string dbHostName;
/**
 * Port für den Datenbankserver
 */ 
string dbPortNo;
/**
 * Genutztes Schema innerhalb der Datenbank.
 */ 
string dbSchema;
/**
 * Username zur Anmeldung an der Datenbank.
 */
string dbUserName;
/**
 * Passwort zur Anmeldung an der Datenbank.
 */
string dbPassWord;
/**********************************************
 * FHEM
 **********************************************/
/**
 * Hostname für den FHEM Server
 */ 
string fhemHostName;
/**
 * Port für den FHEM Server
 * @note Über diesen TCP Port werden Steuerbefehle vom rf24hubd an FHEM geschickt. 
 */
string fhemPortNo;
/**
 * fhemSet: True wenn fhemHostName gesetzt ist, sonst flase
 */
bool fhemHostSet;
/**
 * fhemSet: True wenn fhemPortSet gesetzt ist, sonst flase
 */
bool fhemPortSet;

/**********************************************
 * GENERIC
 **********************************************/
/**
 * startDaemon: True wenn der Programmaufruf mit -d oder --daemon erfolgte, sonst false
 */ 
bool startDaemon = false;
/**
 * startSniffer: True wenn der Programmaufruf mit -S oder --sniffer erfolgte, sonst false
 */ 
bool startSniffer = false;
/**
 * startScanner: True wenn der Programmaufruf mit -s oder --scanner erfolgte, sonst false
 */ 
bool startScanner = false;
/**
 * Beinhaltet das beim Programmaufruf übergebene Scanlevel (0..9)
 */
int setScanLevel = 0;
/**
 * Beinnhaltet den Kanal der gescannt werden soll (1..125); 0 is deaktiviert
 */ 
int channelScanner_Channel = 0;

/**
 * Constructor der Klasse Cfg
 *
 * @param _prgName: Ein String mit dem Namen des aufrufenden Programms.
 * @param _prgVersion: Ein String mit der Programmversion
 *
 */
Cfg(string _prgName, string _prgVersion);

/**
 * Gibt Nutzungsinformationen zum Programm aus
 *
 * @param prgName: Ein String mit dem Namen des aufrufenden Programms.
 *
 */
void usage(const char* prgName);

/**
 * Verarbeitet die Parameter, die beim Start übergeben wurden. 
 * Zusätzlich wird hier das Config-File ausgelesen und verarbeitet.
 * 
 * @param prgName: Ein String mit dem Namen des aufrufenden Programms.
 * @param argc: Anzahl der Aufrufparameter
 * @param argv: Ein Array mit den Aufrufparametern
 * 
 */ 
void processParams(const char* prgName, int argc, char* argv[]);

/**
 * Gibt die Configuration für das Programm rf24gwd aus.
 */
void printConfig_gw (void);

/**
 * Gibt die Configuration für das Programm rf24hubd aus.
 */
void printConfig_hub (void);

/**
 * Gibt die Configuration für die Datenbank aus.
 */
void printConfig_db (void);

/**
 * Setzt ein PID File
 * 
 * @param pidFileName: Dateiname mit absulutem Pfad
 * @return True wenn PID-File gesetzt wurde, sonst False
 */
bool setPidFile(string pidFileName);

/**
 * Löscht ein PID-File 
 * 
 * @param pidFileName: Dateiname mit absulutem Pfad
 */
void removePidFile(string pidFileName);

/**
 * Prüft ob ein PID-File gesetzt ist.
 * Bei gesetztem PID-File wird zusätzlich eine Warnmeldung "PIDFILE: <PIDFILE Name> exists" in den STDERR geschrieben.
 * 
 * @param pidFileName: Dateiname mit absulutem Pfad
 * @return True wenn PID-File vorhanden ist, sonst False
 */
bool checkPidFileSet(string pidFileName);

};

#endif  //CFG_H
