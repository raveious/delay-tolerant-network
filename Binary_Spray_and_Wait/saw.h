#ifndef __BIN_SPRAY_AND_WAIT_HEADER__
#define __BIN_SPRAY_AND_WAIT_HEADER__

#define SPRAY_STACK 5
#define BROADCAST_INTERVAL 500 // in milli-seconds

#define BROADCAST_STATE   0x01
#define WILLING_STATE     0x02
#define SPRAYED_STATE     0x03
#define DESTINATION_STATE 0x04

// TODO update this to be more like a header than a whole packet
typedef nx_struct SprayAndWaitMsg {
	nx_uint32_t uuid;
	nx_uint16_t nodeID;
	nx_uint16_t value;
	nx_uint16_t stacks;
	nx_uint8_t status;
} SprayAndWaitMsg;

#endif
