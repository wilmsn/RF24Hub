#ifndef RF24_CONFIG_H   
#define RF24_CONFIG_H

/***********************************
 * Definition der RF24 Parameter
 * Diese Datei wird vom Hub und
 * von den Nodes eingebunden !!!
 **********************************/

#define RF24_CHANNEL        92
#define RF24_SPEED          RF24_250KBPS
#define RF24_HUB2NODE       { 0xf0, 0xcc, 0xfc, 0xcc, 0xcc}
#define RF24_NODE2HUB       { 0x33, 0xcc, 0xfc, 0xcc, 0xcc}
#define NODE_DATTYPE        uint8_t
#define ONR_DATTYPE         uint8_t

/***********************************
 * Definition der Kontrollregister
 **********************************/


/***********************************
 * Definition der Register
 **********************************/
#define REG_BATT            101
#define REG_VOLTFAC         102
#define REG_VOLTOFF         103
#define REG_LOWVOLTLEV      104
#define REG_SLEEPTIMEADJ    106
#define REG_SLEEPTIMEKOR    107
#define REG_TRANSREG        111
#define REG_MONITOR         112
#define REG_DISPLAY         113
#define REG_SLEEPTIME       114
#define REG_EMPTYLOOPS      115
#define REG_SENDDELAY       116
#define REG_SNDCNTN         117
#define REG_SNDCNTS         118
#define REG_LOWVOLTINT      119
#define REG_PALEVEL         124
#define REG_SW              125
/***********************************
 * Definition der Nachrichtenarten
 **********************************/

// Payload Message Flags
// Leeres Flag
#define PAYLOAD_FLAG_EMPTY       0b00000000
// Flag zeigt an das diese Nachricht die letzte (Teil-)Nachricht war
#define PAYLOAD_FLAG_LASTMESSAGE 0b00000001
// Flag zeigt an das der Node Support benötigt (z.B. durch niedrige Spannung)
#define PAYLOAD_FLAG_NEEDHELP    0b10000000
// ****** Payload Message Type ******
// Nachricht ist ein Init
// Datenformat: komprimiert Channel und float
// Ursprung: Node; Empfänger: Hub
#define PAYLOAD_TYPE_INIT        1

// Nachricht ist ein Heatbeat
// Datenformat: komprimiert Channel und float
// Ursprung: Node; Empfänger: Hub
#define PAYLOAD_TYPE_HB          51
// Nachricht ist eine Quittung für einen Heatbeat,
// Datenformat: Alle 6 data Felder sind leer (0)
// Ursprung: Hub; Empfänger: Node
#define PAYLOAD_TYPE_HB_RESP      52

// Daten Nachricht
// Datenformat: komprimiert Channel und float
// Ursprung: Hub; Empfänger: Node
#define PAYLOAD_TYPE_DAT         61
// Antwort auf Daten Nachricht
// verarbeitete Daten werden unverändert zurückgesand
// Datenformat: komprimiert Channel und float
// Ursprung: Node; Empfänger: Hub
#define PAYLOAD_TYPE_DATRESP     62
// Antwort auf Datenantwort(PAYLOAD_TYPE_DATRES) / Stoppnachricht
// Daten sind leer
// Datenformat: komprimiert Channel und float
// Ursprung: Hub; Empfänger: Node
#define PAYLOAD_TYPE_DATSTOP     63

/*************************************
 * Ein Ping geht immer vom Node aus. 
 * Durch die unterschiedlichen
 * Sendeleistungen kann die Qualität
 * der Funkverbindung beurteilt werden
 ************************************/
// Nachricht ist ein Ping.
// Sendeleistung ist Minimal (-18 dBm)
#define PAYLOAD_TYPE_PING_POW_MIN 101
// Nachricht ist ein Ping.
// Sendeleistung ist Low (-12 dBm)
#define PAYLOAD_TYPE_PING_POW_LOW 102
// Nachricht ist ein Ping.
// Sendeleistung ist High (-6 dBm)
#define PAYLOAD_TYPE_PING_POW_HIGH 103
// Nachricht ist ein Ping.
// Sendeleistung ist Max ( 0 dBm)
#define PAYLOAD_TYPE_PING_POW_MAX 104
// Nachricht ist ein Ping.
// Ende des Tests Sendeleistung ist Max ( 0 dBm)
#define PAYLOAD_TYPE_PING_END 105

/**
 * @typedef payload_t Die Datenstructur zur Übertragung der Daten zwischen Gateway und Node
 * In der aktuellen Struktur können bis zu 6 Transprortwerte (Vereinigeung von Channel und Wert) transportiert werden.
 */
// Structure of our payload
typedef struct {   // Our payload can be 32 byte max.
    // Die Node_ID ist der eindeutige Identifizierer für einen Node.
    // Hier können die Nodes 1..255 genutzt werden (8 Bit Begrenzung)
    NODE_DATTYPE    node_id;         
    // Die MSG_ID ist der eindeutige Identifizierer einer Nachricht.
    // Muss einen Nachricht wiederholt werden, wird sie hochgezählt.
    uint8_t     msg_id;          
    // Art der Nachricht, Definition siehe oben.
    uint8_t     msg_type;        
    // Nachrichtenflags, Definition siehe oben.
    uint8_t     msg_flags;   
    // Ordernummern werden im Hub verwaltet.
    // Auf eine Anfrage vom Hub wird immer mit der selben ORDER_NO geantwortet
    // Nachrichten, die ihren Ursprung im Node haben ( z.B. Heatbeatmessages ) 
    // erhalten die ORDER_NO "0"    
    ONR_DATTYPE     orderno;         
    // Die heartbeatno wird bei jedem neuen Heartbeat hochgezählt
    // Da es sich um eine 8 Bit Zahl handelt wird der gültige Bereich für normale Heartbeats von 1...200 festgelegt
    // Der Bereich 201...255 gilt für besondere Nachrichten (z.B. Initialisierung )
    uint8_t     heartbeatno;      
    // noch nicht genutzt
    uint8_t     reserved2;      
    // noch nicht genutzt
    uint8_t     reserved3;      
    // Datenpaket 1 (32Bit)
    uint32_t    data1;         
    // Datenpaket 2 (32Bit)
    uint32_t    data2;         
    // Datenpaket 3 (32Bit)
    uint32_t    data3;         
    // Datenpaket 4 (32Bit)
    uint32_t    data4;         
    // Datenpaket 5 (32Bit)
    uint32_t    data5;         
    // Datenpaket 6 (32Bit)
    uint32_t    data6;         
} payload_t;

// structure of UDP data
typedef struct {
  uint16_t      gw_no;         // the number of the sending gateway
  payload_t     payload;      // the payload to send forward
} udpdata_t;

#endif
