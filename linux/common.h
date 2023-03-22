/**
 *
 */

/**
 * @brief Common functions used in rf24gwd and rf24hubd
*/

#ifndef _COMMON_H_   /* Include guard */
#define _COMMON_H_

#include <stdint.h>
#include <cstring>
#include <ctype.h> 
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstdio>
#include <netinet/in.h>
#include <fcntl.h>
#include "config.h"
#include "dataformat.h"
#include "rf24_config.h"
#include "version.h"

enum sockType_t { TCP, UDP};

struct TnData_t {
        char tntext[DEBUGSTRINGSIZE];
        int  tn_socket;    
        char address[IPADDRESSLENGTH];
};

struct TnMsg_t {
        long mtype;
        struct TnData_t TnData;
};

/**
 * Übergabedaten für den Thread "receive_tn_in"
 */
struct ThreadTnData_t {
        int tnsocket;
        char address[IPADDRESSLENGTH];
};

static const char date_format1[] = "%d.%m.%Y %H:%M:%S";
static const char date_format2[] = "%Y.%m.%d %H:%M:%S";

/**
 * utime2str: konvertiert den übergebenen
 * Unix Zeitstempel utime in einen String
 * Ist utime = 0 wird die aktuelle Zeit genommen.
 * Für form gilt:
 * 1 => 20.04.2020 18:38:17
 * 2 => 2020.04.20 18:38:17
 * Wichtig: Es muss ein Speicherplatz buf zur Aufnahme 
 * des Strings ( Grösse 20 Byte ) übergeben werden!
 * @param utime Unix Timestamp
 * @param buf Ein Puffer zur Aufnahme des Ausgabestrings. Muss groß genug sein um den Ausgabestring komplett aufnehem zu können.
 * @param format Bestimmt das Ausgabeformat des Zeitstrings: "1" => "Tag.Monat.Jahr Stunde:Minute:Sekunde"; "2" => "Jahr.Monat.Tag Stunde:Minute:Sekunde"
 * @return Der Zeitstring
 */
char* utime2str(time_t utime, char* buf, uint8_t format);

/**
 * Erzeugt einen aktuellen Zeitstempel zur Verwendung in Log-Ausgaben.
 * Wichtig: Es muss ein Buffer übergeben werden, der diesen Zeitstempel komplett aufnehmen kann!
 * @param Der Buffer zur Aufnahme des Zeitstempels
 * @return Der Zeitstempel als String
 */
char* ts(char* buf);

/**
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 * @param s Eingabestring (schmutzig)
 * @return Ausgabestring (gereinigt)
 */
char * trim (char * s);

/**
 * Ein Wrapper für malloc() mit folgenden Erweiterungen:
 * Je nach aktuell gesetztem Verboselevel erfolgen Ausgaben
 * @param verboseLevel Der aktuell gesetzte Verboselevel
 * @param msgTxt Der Ausgabetext für Verbosemeldungen
 * @param size Die mittels malloc() zu allokierende Speichergröße
 * @param timestring Der aktuelle Timestring z.B. durch ts(buf) erzeugt
 * @return Ein leerer String mit der Größe <size> 
 */
char* alloc_str(uint16_t verboseLevel, const char* msgTxt, size_t size, char* timestring);

/**
 * Ein Wrapper für free() mit folgenden Erweiterungen:
 * Je nach aktuell gesetztem Verboselevel erfolgen Ausgaben
 * @param verboseLevel Der aktuell gesetzte Verboselevel
 * @param msgTxt Der Ausgabetext für Verbosemeldungen
 * @param str Ein zuvor mit alloc_str reservierter String(Speicherbereich)
 * @param timestring Der aktuelle Timestring z.B. durch ts(buf) erzeugt
 */
void free_str(uint16_t verboseLevel, const char* msgTxt, char* str, char* timestring);

/**
 * Bildet den Unix Timestamp erweitert um Millisekunden ab
 * @return Der erweiterte Timestamp
 */
uint64_t mymillis(void);

/**
 * Gibt das aktuelle Verboselevel aus
 * @param buf Hier muss ein gültiges CHAR Array übergeben werden
 * @param verboseLevel Der aktuelle Verboselevel
 */
char* printVerbose(uint16_t verboseLevel, char* buf);

/**
 * Ermittelt den Binärwert eines neuen Verboselevels
 * @param verboseLevel Der alte Verboselevel als binärwert
 * @param verboseSet Der neue Verboselevel als String, z.B. rf24
 * @return Der neu Verboselevel als binärwert
 */
uint16_t decodeVerbose(uint16_t verboseLevel, char* verboseSet);

/**
 * Verschickt eine UDP Nachricht
 * @param host Der Empfängerhost
 * @param port Der UDP Port auf dem Empfängerhost
 * @param udpdata Die zu übertragenen Daten als Datentyp udpdata_t
 */
void sendUdpMessage(const char* host, const char* port, udpdata_t * udpdata );

/**
 * Öffnet ein Socket auf dem localen Host
 * @param port Der zu öffnede Port
 * @param address Eine leere Varaible vom Typ 
 * @param handle Ein Pointer auf eine INT Variable - Der Filehandler für dieses Socket
 * @param sockType Typ des Sockets: UDP oder TCP
 */
bool openSocket(const char* port, struct sockaddr_in *address, int* handle, sockType_t sockType );

/**
 * Druckt den übergebenen payload in den STDIO
 * @param ts Ein gültiges array of char mit dem Timestamp
 * @param header Ein vor dem Payload zu druckener Text
 * @param payload der zu druckende Payload
 */ 
void printPayload(char* ts, const char* header, payload_t* payload);

#endif // _RF24HUBD_COMMON_H_

