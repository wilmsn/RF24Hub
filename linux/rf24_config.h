#ifndef RF24_CONFIG_H   
#define RF24_CONFIG_H

/**
 * @file rf24_config.h
 * @brief Definition der RF24 Parameter
 * Diese Datei wird vom GW, vom Hub und von den Nodes eingebunden !!!
 * Hier sind alle Parameter zum RF24-Netzwerk und die Register definiert.
 * Zusätzlich sind hier die Payload Struktur und diee Udpdata Struktur definiert.
 */

#include <time.h>

/** @defgroup rf24makros RF24 Makros
 *  @addtogroup rf24makros
 *  @brief Definition der RF24 Netzwerk Einstellungen
 *  @{
 */
///Der verwendete RF24 Funkkanal
#define RF24_CHANNEL        92
/// Die Übertragungsgeschwindigkeit
#define RF24_SPEED          RF24_250KBPS
/// Der Netzwerkschlüssel Hub zum Node
#define RF24_HUB2NODE       { 0xf0, 0xcc, 0xfc, 0xcc, 0xcc}
/// Der Netzwerkschlüssel Node zum Hub
#define RF24_NODE2HUB       { 0x33, 0xcc, 0xfc, 0xcc, 0xcc}
/// Der Datentyp für die Node_ID. Ist aktuell auf 1...255 festgelegt. Werden mehr Nodes benötigt, kann der Datentyp hier zentral umgestellt werden.
#define NODE_DATTYPE        uint8_t
/// Der Datentyp für die Ordernummer. Auch hier eine zentrale Festlegung des Datentyps, der ggf. die Umstellung vereinfacht.
#define ONR_DATTYPE         uint8_t
/// @}

/**
 * @defgroup reggrp Definition der Registergruppen
 * @addtogroup reggrp
 * Definition der Channel Verteilung (generische Definitionen und Sonderbehandlung)
 * Achtung: Bereiche müssen streng getrennt sein und dürfen sich nicht überschneiden 
 * Es werden folgende Bereiche unterschieden:<br>
 * SENSOR_CHANNEL: In diesem Bereich sind die normalen Sensoren und Aktoren angesiedelt.<br>
 * SENSOR_BATT: Ein Sensor mit Sonderbehandlung für die Batterie im Bereich der normalen Sensoren<br>
 * REG_NOSTORE: Hier sind die Register angesiedelt deren Inhalte (im Node) einmal verarbeitet und dann vergessen werden.<br>
 * REG_NORMAL: Normale Register, die geschrieben, gelesen und wieder überschrieben werden können.<br>
 * REG_READONLY: Register, die nur gelesen werden können.<br>
 * 
 * @{
 */
/// Erster normaler Sensorchannel
#define SENSOR_CHANNEL_FIRST        1
/// Letzter normaler Sensorchannel
#define SENSOR_CHANNEL_LAST         79
/// Erstes Register, das vom Hub nicht gespeichert wird
#define REG_NOSTORE_FIRST           80
/// Letztes Register, das vom Hub nicht gespeichert wird
#define REG_NOSTORE_LAST            89
/// Erstes Register gilt die normale Behandlung
#define REG_NORMAL_FIRST            90
/// Letztes Register gilt die normale Behandlung
#define REG_NORMAL_LAST             120
/// Erstes Read only Register
#define REG_READONLY_FIRST          121
/// Letztes Read only Register
#define REG_READONLY_LAST           127
/// @}

/**
 * @defgroup reg Definition der Register
 * @addtogroup reg
 * Definition der Kontrollregister<br>
 * Nicht jedes Register ist in jedem Node verfügbar. Dies ist abhängig von der verbauten Hardware und der eingespielten Software.
 * @{
 */
/// Sensor für den Spannungswert der Batterie auf Channel 79
#define SENSOR_BATT                 79
/// Einmalige Korrektur der Schlafzeit in Sekunden
/// Format: int16_t; Wertebereich: -1000 ... 1000
#define	REG_SLEEPTIME_KOR           80
/// Schreiben eines beliebigen Wertes startet den Registertransfer zum Hub.
/// Format: uint16_t; Wertebereich: beliebig
#define	REG_TRANSREG	            81
/// Schreiben eines beliebigen Wertes startet die PA Messung.
/// Format: uint16_t; Wertebereich: beliebig
#define	REG_TRANSPA                 82
/// Schreiben eines beliebigen Wertes in dieses Register setzt alle EEPROM Werte auf Default zurück.
/// Format: uint16_t; Wertebereich: beliebig
#define REG_DEFAULT                 83
/// EntladeSpannung Level 1<br>
/// Bei Überschreiten dieser Spannung wird der ATMega nicht mehr in den Schlafmodus versetzt.
/// Format: float; Wertebereich: 1...20
#define	REG_DISCHARGE_LEV1          91
/// EntladeSpannung Level 2<br>
/// Bei Überschreiten dieser Spannung wird die Batterie zusätzlich über einen Widerstand entladen.
/// Format: float; Wertebereich: 1...20
#define	REG_DISCHARGE_LEV2          92
/// EntladeSpannung Level 2<br>
/// Bei Überschreiten dieser Spannung wird die Batterie zusätzlich über einen Widerstand entladen.
/// Format: float; Wertebereich: 1...20
#define	REG_DISCHARGE_LEV3          93
/// Maximal zulässige Differenzspannung bei 2 Batterien.
/// Wird diese Spannung überschritten, wirden die Spannungspegel durch gezieltes laden/entladen angepasst.
#define	REG_LOAD_BALLANCER	    94
/// Kontrastpegel (nur bei Displays)
#define	REG_CONTRAST                95
/// Helligkeitspegel (nur bei Displays)
#define	REG_BRIGHTNES               96
/// Spannungskorrekturfaktor
/// U = Umess * factor + offset
/// Format: float; Wertebereich: 0.1 ... 10; Default: 1
#define	REG_VOLT_FAC                110
/// Spannungsoffset
/// U = Umess * factor + offset
/// Format: float; Wertebereich: -10 ... 10; Default: 0
#define	REG_VOLT_OFF                111
/// Low Voltage Level
/// Bei unterschreiten dieser Spannung geht der Node in den Low Voltage Modus. Dann gilt die SLEEPTIME_LV.
/// Format: float; Wertebereich: 1 ... 5; Default: 2
#define	REG_VOLT_LV                 112
/// Schlafzeit im Low Voltgage Modus
/// Der einstellbare Bereich liegt von 10 Sekunden bis 9 Stunden (=32400 Integer Begrenzung)
/// Format: uint16_t; Wertebereich: 10 ... 32400; Default: 900
#define	REG_SLEEPTIME_LV	    113
/// Normale Schlafzeit
/// Der einstellbare Bereich liegt von 10 Sekunden bis 9 Stunden (=32400 Integer Begrenzung)
/// Format: uint16_t; Wertebereich: 10 ... 32400; Default: 300
#define	REG_SLEEPTIME               114
/// Feinjustierung der Sekunden für den Tiefschlaf
/// Format: uint16_t; Wertebereich: 500 ... 2000; Default: 1000
#define	REG_SLEEP4MS_FAC            115
/// Verzögerung zwischen zwei Sendungen/Sendewiederholung
/// Format: uint16_t; Wertebereich: 50 ... 1000; Default: 500
#define	REG_SENDDELAY               116
/// Maximale Anzahl von Sendeversuchen für normale Sendungen
/// Format: uint16_t; Wertebereich: 1 ... 20; Default: 10
#define	REG_MAX_SENDCOUNT           117
/// Maximale Anzahl von Sendeversuchen für stopp Sendungen
/// Format: uint16_t; Wertebereich: 1 ... 20; Default: 3
#define	REG_MAX_STOPCOUNT           118
/// Anzahl der Loops ohne Sendungen
/// Format: uint16_t; Wertebereich: 0 ... 20; Default: 0
#define	REG_EMPTYLOOPS              119
/// Sendefeldstärke
/// Legt die Felstärke fest mit der dieser Node sendet. 1 = Min(-18dbm), 2 = Low(-12dbm), 3 = High(-6dbm), 4 = Max(0dbm)
/// Format: uint16_t; Wertebereich: 0 ... 3; Default: 3
#define	REG_PALEVEL                 120
/// Empfangsfeldstärke
/// Legt die Felstärke fest mit der dieser Node vom Hub empfangen wurde.
/// 0 = Min(-18dbm), 1 = Low(-12dbm), 2 = High(-6dbm), 3 = Max(0dbm)
/// Format: uint16_t; Wertebereich: 0 ... 3;
/// Diese Information wird nicht zum Node übertragen
#define REG_RECLEVEL                121
/// Softwarestand
/// Wird im Format XYZZ übertragen: X=Release, Y=Subrelease, ZZ=Änderungszähler
/// Format: uint16_t; Wertebereich: 0 ... 20; Default: 0
#define	REG_SW                      125
/// @}

/**
 * @defgroup msgflg Definition der Messageflags
 * @addtogroup msgflg
 * Definition der Messageflags => Payload Message Flags
 * @{
 */

/// Leeres Messageflag
#define PAYLOAD_FLAG_EMPTY          0b00000000
/// Flag zeigt an das diese Nachricht die letzte (Teil-)Nachricht war
#define PAYLOAD_FLAG_LASTMESSAGE    0b00000001
/// Flag zeigt an das die Spannung im Node kritisch ist
#define PAYLOAD_FLAG_LOWVOLTAGE     0b00000010
///@}


/**
 * @defgroup msgtyp Definition der Messagetypen
 * @addtogroup msgtyp
 * Nachrichten werden in abhängigkeit von ihrem Nachrichtentyp im Hub unterschiedlich behandelt.
 * <b>Sonderfall Ping zur Feldstärkenmessung:</b> 
 * Ein Ping geht immer vom Node aus. Durch die unterschiedlichen Sendeleistungen kann die Qualität der Funkverbindung beurteilt werden.
 * 
 * @{
*/
 
/// Nachricht ist ein Init
/// Ursprung: Node; Empfänger: Hub
#define PAYLOAD_TYPE_INIT           1

/// Nachricht ist ein Messwert aus einem ESPNode
/// Ursprung: ESPNode; Empfänger: Hub
/// Diese Nachricht wird nicht vom Hub beantwortet
#define PAYLOAD_TYPE_ESP            41

/// Nachricht ist ein initialer Heatbeat
/// Ursprung: Node; Empfänger: Hub
#define PAYLOAD_TYPE_HB             51

/// Nachricht ist eine Quittung für einen Heatbeat,
/// Alle 6 data Felder sind leer (0)
/// Ursprung: Hub; Empfänger: Node
#define PAYLOAD_TYPE_HB_RESP        52

/// Nachricht ist ein Folge Heatbeat
/// Ursprung: Node; Empfänger: Hub
#define PAYLOAD_TYPE_HB_F           55

/// Nachricht ist eine Quittung für einen Folge Heatbeat,
/// Alle 6 data Felder sind leer (0)
/// Ursprung: Hub; Empfänger: Node
#define PAYLOAD_TYPE_HB_F_RESP      56

/// Daten Nachricht, erfolgt als Antwort auf einen Heartbeat wenn der Hub dem Node etwas mitteilen möchte.
/// Ursprung: Hub; Empfänger: Node
#define PAYLOAD_TYPE_DAT            61

/// Antwort auf Daten Nachricht
/// verarbeitete Daten werden unverändert zurückgesand
/// Ursprung: Node; Empfänger: Hub
#define PAYLOAD_TYPE_DATRESP        62

// Antwort auf Datenantwort(PAYLOAD_TYPE_DATRES) / Stoppnachricht
// Alle Datenfelder sind leer
// Ursprung: Hub; Empfänger: Node
#define PAYLOAD_TYPE_DATSTOP        63

/// Nachricht ist ein Ping.
/// Sendeleistung ist Minimal (-18 dBm)
#define PAYLOAD_TYPE_PING_POW_MIN   101
/// Nachricht ist ein Ping.
/// Sendeleistung ist Low (-12 dBm)
#define PAYLOAD_TYPE_PING_POW_LOW   102
/// Nachricht ist ein Ping.
/// Sendeleistung ist High (-6 dBm)
#define PAYLOAD_TYPE_PING_POW_HIGH  103
/// Nachricht ist ein Ping.
/// Sendeleistung ist Max ( 0 dBm)
#define PAYLOAD_TYPE_PING_POW_MAX   104
/// Nachricht ist ein Ping.
/// Ende des Tests Sendeleistung ist Max ( 0 dBm)
#define PAYLOAD_TYPE_PING_END       105
/// @}

/**
 * @typedef payload_t Die Datenstructur zur Übertragung der Daten zwischen Gateway und Node
 * In der aktuellen Struktur können bis zu 6 Transprortwerte (Vereinigeung von Channel und Wert) transportiert werden.
 * Der nrf24l01 kann einen Payload von 32byte transportieren. dieser Datentyp ist genau 32bylte lang.
 */
typedef struct {
/// Die Node_ID ist der eindeutige Identifizierer für einen Node.
/// Aktuell können hier die Nodes 1..255 genutzt werden (8 Bit Begrenzung)
/// Damit der Datentyp einfach gewechselt werden kann ist er nur indirekt festgelegt.   
    NODE_DATTYPE    node_id;         
/// Die MSG_ID ist der eindeutige Identifizierer einer Nachricht.
/// Muss einen Nachricht wiederholt werden, wird sie hochgezählt.
    uint8_t         msg_id;          
/// Art der Nachricht, Definition siehe Nachrichtentyp.
    uint8_t         msg_type;        
/// Nachrichtenflag, Definition siehe Nachrichtenflags.
    uint8_t         msg_flags;   
/// Ordernummern werden im Hub verwaltet und dort nach jeder Order hochgezählt.
/// Auf eine Anfrage vom Hub wird immer mit der selben Ordernummer geantwortet.
/// Nachrichten, die ihren Ursprung im Node haben ( z.B. Heatbeatmessages ) 
/// erhalten die Ordernummer "0", Ordernummern größer 250 diesen zur Messung des PA Levels.    
    ONR_DATTYPE     orderno;         
/// Die heartbeatno wird bei jedem neuen Heartbeat hochgezählt
/// Da es sich um eine 8 Bit Zahl handelt wird der gültige Bereich für normale Heartbeats von 1...200 festgelegt
/// Der Bereich 201...255 gilt für besondere Nachrichten (z.B. Initialisierung )
    uint8_t         heartbeatno;      
/// noch nicht genutzt
    uint8_t         reserved1;      
/// noch nicht genutzt
    uint8_t         reserved2;      
/// Datenpaket 1 (32Bit)
    uint32_t        data1;         
/// Datenpaket 2 (32Bit)
    uint32_t        data2;         
/// Datenpaket 3 (32Bit)
    uint32_t        data3;         
/// Datenpaket 4 (32Bit)
    uint32_t        data4;         
/// Datenpaket 5 (32Bit)
    uint32_t        data5;         
/// Datenpaket 6 (32Bit)
    uint32_t        data6;         
} payload_t;

/**
 * @typedef udpdata_t Die Datenstructur zur Übertragung der Daten zwischen Gateway und Hub
 * Im Prinzig ebtspricht diese Struktur der payload_t Struktur erweitert um ein Feld zur Aufnahme der Gateway_id.
 * 
 */
typedef struct {
/// Die eindeutige Gateway ID
  uint16_t      gw_no;         // the number of the sending gateway
/// Die Payloadstruktur wie unter payload_t definiert.
  payload_t     payload;      // the payload to send forward
/// Der Unix Timestamp
  time_t        utime;
} udpdata_t;

#endif
