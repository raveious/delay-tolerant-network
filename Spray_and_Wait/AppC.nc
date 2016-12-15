#include "saw.h"

module AppC {
	uses {
		interface Leds;
		interface Boot;
		interface AMSend;
		interface Receive;
	}
}
implementation {
	event void Boot.booted() {
		// Only node 0 will try to send data, everyone else will be a relay
		if (TOS_NODE_ID == 0) {
			message_t data;
			SprayAndWaitMsg_t* ptr = (SprayAndWaitMsg_t*)(call AMSend.getPayload(&data, sizeof(SprayAndWaitMsg_t)));

			// Try sending data to node 10
			call AMSend.send(10, &data, sizeof(SprayAndWaitMsg_t));
		}
	}

	event void AMSend.sendDone(message_t* msg, error_t err) {
	}

	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len) {
		// denote that a packet was received
		call Leds.led2On();

		return msg;
	}
}
