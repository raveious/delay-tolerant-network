#ifndef __SPRAY_AND_WAIT_HEADER__
#define __SPRAY_AND_WAIT_HEADER__

#define SPRAY_WIDTH 2
#define BROADCAST_INTERVAL 500 // in milli-seconds

typedef enum route_state_t {
	BROADCAST,
	WILLING,
	SPRAYED,
	DESTINATION
} route_state;

// TODO update this to be more like a header than a whole packet
typedef nx_struct SprayAndWaitMsg {
	nx_uint32_t uuid;
	nx_uint16_t nodeID;
	nx_uint16_t value;
	route_state status;
} SprayAndWaitMsg;

#endif
