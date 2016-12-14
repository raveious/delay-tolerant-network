#ifndef __SPRAY_AND_WAIT_HEADER__
#define __SPRAY_AND_WAIT_HEADER__

#define SPRAY_WIDTH 2
#define BROADCAST_INTERVAL 500 // in milli-seconds
#define MAX_PACKET_BUFFER 2

#define AM_SPRAYNWAIT 6

#define BROADCAST_STATE   0x01
#define WILLING_STATE     0x02
#define SPRAYED_STATE     0x03
#define DESTINATION_STATE 0x04

#define AUTO_ACK_BACK TRUE

// TODO update this to be more like a header than a whole packet
typedef nx_struct SprayAndWaitMsg {
	nx_uint32_t uuid;
	nx_uint16_t srcNodeID;
	nx_uint16_t destNodeID;
	nx_uint8_t rout_stat;
	nx_uint8_t ACK;
} SprayAndWaitMsg_t;

#endif
