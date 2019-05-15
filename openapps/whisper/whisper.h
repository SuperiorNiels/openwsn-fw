#ifndef __WHISPER_H
#define __WHISPER_H

/**
\addtogroup AppCoAP
\{
\addtogroup whisper
\{
*/

#include "opencoap.h"
#include "IEEE802154.h"

//=========================== define ==========================================

#define SIXTOP_MAX_LINK_SNIFFING 60 // number of links to keep track of

#define WHISPER_STATE_IDLE          0x00
#define WHISPER_STATE_SIXTOP        0x01
#define WHISPER_STATE_DIO           0x02
#define WHISPER_STATE_WAIT_COAP     0x03
#define WHISPER_STATE_SEND_RESULT   0x04

#define WHISPER_COMMAND_DIO             0x01
#define WHISPER_COMMAND_SIXTOP          0x02
#define WHISPER_COMMAND_LINK_INFO       0x03
#define WHISPER_COMMAND_NEIGHBOURS      0x04
#define WHISPER_COMMNAD_TOGGLE_DIO      0x05
#define WHISPER_COMMNAD_SET_RANKTOSEND  0x06

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
    bool            active; // when true no normal dios will be sent
} whisper_propagating_dio_settings;

typedef struct {
    uint8_t srcId[2];
    uint8_t destId[2];
    uint8_t seqNum;
    bool    active;
} whisper_sixtop_link;

typedef struct {
    whisper_sixtop_link sixtop[SIXTOP_MAX_LINK_SNIFFING];
} whisper_neighbor_info;

typedef struct {
    open_addr_t	target;
    open_addr_t	parent;
    open_addr_t nextHop;
    dagrank_t   rank;
    bool        changeL2src;
} whisper_dio_settings;

typedef struct {
    bool        acceptACKs;
    open_addr_t ACKsrc;
    open_addr_t ACKdest;
} whisper_ack_sniffing;

typedef struct {
    open_addr_t     target;
    open_addr_t     source;
    uint8_t         request_type;
    uint8_t         cellType;
    uint8_t         seqNum;
    cellInfo_ht     cell;
    uint16_t        listMaxCells;
    uint16_t        listOffset;
    bool            waiting_for_response;
    bool            command_parsed;
} whisper_sixtop_request_settings;

typedef struct {
    coap_resource_desc_t desc;
    opentimers_id_t      periodicTimer;
    opentimers_id_t      oneshotTimer;
    uint16_t             timerPeriod;
    uint8_t 			 state;
    open_addr_t          my_addr;
    open_addr_t          controller_addr;
    uint8_t              payloadBuffer[30]; // 30 bytes should be enough
    // Command variables
    whisper_ack_sniffing whisper_ack;
    whisper_dio_settings whisper_dio;
    whisper_sixtop_request_settings whisper_sixtop;
    whisper_neighbor_info neighbors;
    whisper_propagating_dio_settings whisper_propagating_dio;
} whisper_vars_t;

//=========================== prototypes ======================================

void            whisper_init(void);
void            whisper_setState(uint8_t i);
uint8_t         whisper_getState(void);
void            whisper_task_remote(uint8_t* buf, uint8_t bufLen);
void            whisperClearStateCb(opentimers_id_t id); // callback to clean up commands
void            whisperClearGeneratedPackets(); // Function to clear the generated packets by whisper
void            whisperStartCleanUpTimer();

// Whisper propagating dio
bool            whisper_getSendNormalDio();
void            whisper_setDIOPeriod(uint16_t period);
void            whisper_togglePropagatingDios();
void            whisper_setRankForNeighbour(uint8_t* command);

// Whipser Fake dio command
open_addr_t*    getWhisperDIOtarget();
open_addr_t*    getWhisperDIOparent();
open_addr_t*    getWhisperDIOnextHop();
dagrank_t       getWhisperDIOrank();
void            whisperDioCommand(const uint8_t* command);
bool            whisperDIOGetChangeL2src();

// Whisper sixtop
open_addr_t*    getWhisperSixtopSource();
bool            whisperSixtopParse(const uint8_t* command);
void            whisperExecuteSixtop();
void            whisperSixtopResonseReceive(open_addr_t* addr, uint8_t code);
void            whisperSixtopProcessIE(OpenQueueEntry_t* pkt);

bool            whisperSixtopPacketAccept(ieee802154_header_iht *ieee802514_header);
void            whisperGetNeighborInfoFromSixtop(ieee802154_header_iht* header, OpenQueueEntry_t* msg);

// Whisper ACK Sniffing
bool            whisperACKreceive(ieee802154_header_iht* ieee802154_header);

// helper functions
void            whisperUpdateOrAddSixtopLinkInfo(open_addr_t* src, open_addr_t* dest, uint8_t seqNum);
bool            isMatchingSixtopLink(uint8_t link_index, const uint8_t* srcID, const uint8_t* destID);
bool            addSixtopLink(const uint8_t* srcID, const uint8_t* destID, uint8_t seqNum);
bool            getSixtopLinkSeqNum(const uint8_t* srcID, const uint8_t* destID, uint8_t* seqNum);
void            updateSixtopLinkSeqNum(const uint8_t* srcID, const uint8_t* destID, uint8_t seqNum);
void            removeAllSixtopLinksNeighbor(open_addr_t* neighbor);

void            sendCoapResponseToController(uint8_t *payload, uint8_t length);

// Logging (should be removed for openmote build, no printf..)
void            whisper_log(char* msg, ...);
void            whisper_print_address(open_addr_t* addr);

/**
\}
\}
*/

#endif
