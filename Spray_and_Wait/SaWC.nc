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
		interface Leds;
	}
	provides {
		interface AMSend as AppSendQueue;
		interface Receive as AppReceive;
	}
}
implementation {
	uint16_t stored = 0;
	bool busy = FALSE;
	int16_t next = 1;
	SaWQueueItem_t buffer[MAX_PACKET_BUFFER];

	event void Boot.booted() {
		call AMControl.start();

		memset(buffer, 0, sizeof(SaWQueueItem_t) * MAX_PACKET_BUFFER);
	}

	event void AMSend.sendDone(message_t* msg, error_t err) {
		call Leds.led0Off();
		busy = FALSE;
	}

	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len) {
		// TODO check the type of packet, probably reply with another msg

		if (len == sizeof(SprayAndWaitMsg_t)) {
			SprayAndWaitMsg_t* saw = (SprayAndWaitMsg_t*)payload;

			if (saw->rout_stat == BROADCAST_STATE) {
				call Leds.led1On();

			} else if (saw->rout_stat == WILLING_STATE) {

			} else if (saw->rout_stat == SPRAYED_STATE) {
				call Leds.led1Off();
			} else {
				signal AppReceive.receive(msg, payload, len);
			}
		}
		return msg;
	}

	event void BroadcastTimer.fired() {
		// If we have some packets to go out and we're not busy with one already...
		if (stored > 0 && !busy) {

			buffer[next].message.srcNodeID = TOS_NODE_ID;
			if (call AMSend.send(AM_BROADCAST_ADDR, (message_t*) &(buffer[next].message), sizeof(SprayAndWaitMsg_t)) == SUCCESS) {
				call Leds.led0On();
				busy = TRUE;
				next++;
				if (next >= MAX_PACKET_BUFFER) {
					next = 0;
				}
			}
		}
	}

	command error_t AppSendQueue.send(am_addr_t addr, message_t* msg, uint8_t len) {
		if (stored >= MAX_PACKET_BUFFER) {
			return EBUSY;
		} else if (len != sizeof(SprayAndWaitMsg_t)) {
			return FAIL;
		}

		// Put msg into queue for transmission.
		memcpy(msg, &(buffer[stored - 1].message), sizeof(SprayAndWaitMsg_t));

		// Prep the rest of the payload
		buffer[stored - 1].available = SPRAY_WIDTH;
		buffer[stored - 1].message.rout_stat = BROADCAST_STATE;
		buffer[stored - 1].message.destNodeID = addr;

		// Update how many are stored int he queue
		stored++;
	}

	command error_t AppSendQueue.cancel(message_t* msg) {
		// TODO Fill this out to make communication layer complete
		// Cancel in the radio via AMSend.cancel and check the queue
	}

	command uint8_t AppSendQueue.maxPayloadLength() {
		// TODO Fill this out to make communication layer complete
	}

	command void* AppSendQueue.getPayload(message_t* msg, uint8_t len) {
		// TODO Fill this out to make communication layer complete
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
