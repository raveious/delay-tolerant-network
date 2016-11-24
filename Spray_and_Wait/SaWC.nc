#include "Timer.h"
#include "saw.h"

/*
 * This is a implementation of a Spray and Wait routing protocol module.
 */

module SaWC {
	uses {
		interface Boot;
		interface AMSend;
		interface Receive;
		interface SplitControl as AMControl;
		interface Timer<TMilli> as BroadcastTimer;
		interface Packet;
	}
	provides {
		interface AMSend as AMSendQueue;
	}
}
implementation {
	event void Boot.booted() {
		call AMControl.start();
	}

	event void AMSend.sendDone(message_t* msg, error_t err) {
		// TODO Only decrement the available copies
	}

	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len) {
		// TODO check the type of packet, probably reply with another msg
	}

	event void BroadcastTimer.fired() {
		// TODO setup a list of all the packets that still need to go out
	}

	event void AMControl.startDone(error_t err) {
		if (err == SUCCESS) {
			call BroadcastTimer.startPeriodic(BROADCAST_INTERVAL);
		} else {
			call AMControl.start();
		}
	}

	event void AMControl.stopDone(error_t err) {}
}
