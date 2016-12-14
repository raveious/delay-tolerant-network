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
		message_t data;
		SprayAndWaitMsg_t* ptr = (SprayAndWaitMsg_t*)(call AMSend.getPayload(&data, sizeof(SprayAndWaitMsg_t)));

		if (call AMSend.send(10, &data, sizeof(SprayAndWaitMsg_t)) == SUCCESS) {
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
