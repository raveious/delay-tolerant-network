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
		interface AMPacket;
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
	message_t buffer[MAX_PACKET_BUFFER];
	uint16_t available [MAX_PACKET_BUFFER];
	uint32_t cacheUUID [MAX_PACKET_BUFFER];

	event void Boot.booted() {
		call AMControl.start();

		memset(buffer, 0, sizeof(message_t) * MAX_PACKET_BUFFER);
		memset(available, 0, sizeof(uint16_t) * MAX_PACKET_BUFFER);
		memset(cacheUUID, 0, sizeof(uint32_t) * MAX_PACKET_BUFFER);
	}

	event void AMSend.sendDone(message_t* msg, error_t err) {
		call Leds.led0Off();
		busy = FALSE;
	}

	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len) {
		// TODO check the type of packet, probably reply with another msg

		if (len == sizeof(SprayAndWaitMsg_t) && (call AMPacket.isForMe(msg))) {
			SprayAndWaitMsg_t* saw = (SprayAndWaitMsg_t*)payload;
			am_addr_t source = call AMPacket.source(msg);

			if (saw->destNodeID == TOS_NODE_ID) {
				saw->ACK = 1;

				if (call AMSend.send(source, msg, len) == SUCCESS) {
					call Leds.led0On();
					busy = TRUE;
				}
			} else if (saw->rout_stat == BROADCAST_STATE) {
				saw->rout_stat = WILLING_STATE;

				if (call AMSend.send(source, msg, len) == SUCCESS) {
					call Leds.led0On();
					call Leds.led1On();
					busy = TRUE;
				}
			} else if (saw->rout_stat == WILLING_STATE) {
				saw->rout_stat = SPRAYED_STATE;

				if (call AMSend.send(source, msg, len) == SUCCESS) {
					call Leds.led0On();
					busy = TRUE;
				}
			} else if (saw->rout_stat == SPRAYED_STATE) {
				call Leds.led0On();
				call Leds.led1Off();
			} else {
				if (AUTO_ACK_BACK) {

				}
				signal AppReceive.receive(msg, payload, len);
			}
		}
		return msg;
	}

	event void BroadcastTimer.fired() {
		// If we have some packets to go out and we're not busy with one already...
		if (stored > 0 && !busy) {

			if (call AMSend.send(AM_BROADCAST_ADDR, &buffer[next], sizeof(SprayAndWaitMsg_t)) == SUCCESS) {
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
		} else {
			// Put msg into queue for transmission.
			message_t* temp = memcpy(&buffer[stored - 1], msg, sizeof(message_t));

			// prep the rest of the payload
			SprayAndWaitMsg_t* ptr = (SprayAndWaitMsg_t*)(call Packet.getPayload(temp, sizeof(SprayAndWaitMsg_t)));

			// TODO Make UUID actually useful with a random number or something
			ptr->uuid = 1;
			ptr->destNodeID = TOS_NODE_ID;
			ptr->srcNodeID = addr;
			ptr->rout_stat = BROADCAST_STATE;
			ptr->ACK = 0;

			// Update how many are stored int he queue
			available[stored - 1] = SPRAY_WIDTH;
			stored++;

			return SUCCESS;
		}
	}

	command error_t AppSendQueue.cancel(message_t* msg) {
		// TODO Fill this out to make communication layer complete
		// Cancel in the radio via AMSend.cancel and check the queue
	}

	command uint8_t AppSendQueue.maxPayloadLength() {
		return sizeof(SprayAndWaitMsg_t);
	}

	command void* AppSendQueue.getPayload(message_t* msg, uint8_t len) {
		SprayAndWaitMsg_t* ptr = (SprayAndWaitMsg_t*) (call Packet.getPayload(msg, sizeof(SprayAndWaitMsg_t)));

		return ptr;
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
