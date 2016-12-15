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
	int16_t next = 0;
	message_t buffer[MAX_PACKET_BUFFER];
	uint16_t available [MAX_PACKET_BUFFER];
	uint32_t cacheUUID [MAX_PACKET_BUFFER];

	int16_t cacheCheck(uint32_t uuid) {
		int16_t i;
		// check if we have already RX this packet
		for (i = 0; i < stored; i++) {
			if (cacheUUID[i] == uuid) {
				// Do nothing if we already have that packet
				return i;
			}
		}
		return -1;
	}

	event void Boot.booted() {
		call AMControl.start();

		memset(buffer, 0, sizeof(message_t) * MAX_PACKET_BUFFER);
		memset(available, 0, sizeof(uint16_t) * MAX_PACKET_BUFFER);
		memset(cacheUUID, 0, sizeof(uint32_t) * MAX_PACKET_BUFFER);
	}

	event void AMSend.sendDone(message_t* msg, error_t err) {
		call Leds.led0Off();
		call Leds.led1Off();
		busy = FALSE;
	}

	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len) {
		call Leds.led2Toggle();
		if (len == sizeof(SprayAndWaitMsg_t) && (call AMPacket.isForMe(msg))) {
			SprayAndWaitMsg_t* saw = (SprayAndWaitMsg_t*)payload;
			am_addr_t source = call AMPacket.source(msg);
			int16_t uuid_index = cacheCheck(saw->uuid);


			// TODO Check for Ack back on
			if (saw->rout_stat == BROADCAST_STATE && uuid_index < 0) {
				if (saw->destNodeID == TOS_NODE_ID && AUTO_ACK_BACK) {
					saw->ACK = 1;
					saw->rout_stat = DESTINATION_STATE;

					signal AppReceive.receive(msg, payload, len);
				} else {
					saw->rout_stat = WILLING_STATE;
					call Leds.led1On();
				}

				call Leds.led2Toggle();
				if (call AMSend.send(source, msg, len) == SUCCESS) {
					call Leds.led0On();
					busy = TRUE;
				}
			} else if (saw->rout_stat == WILLING_STATE && uuid_index >= 0 && available[uuid_index] > 0) {
				saw->rout_stat = SPRAYED_STATE;

				// Spray this node
				if (call AMSend.send(source, msg, len) == SUCCESS) {
					call Leds.led1On();
					call Leds.led0On();
					busy = TRUE;

					// Update the number of available copies
					available[uuid_index]--;

					// TODO clean out the packet
					// if (available[uuid_index] == 0) {
					// 	cacheUUID[uuid_index] = 0;
					// 	memset(&buffer[uuid_index], 0, sizeof(message_t));
					// 	memcpy(&buffer[uuid_index], &buffer[uuid_index + 1], sizeof(message_t) * (stored - uuid_index));
					// 	stored--;
					// }
				}
			} else if (saw->rout_stat == SPRAYED_STATE) {
				// Sprayed !!!

				saw->rout_stat = DESTINATION_STATE;

				// Add sending to the destination to the constant queue
				memcpy(&buffer[stored], msg, sizeof(message_t));

				available[stored] = 1;
				cacheUUID[stored] = saw->uuid;
				stored++;
			} else if (saw->rout_stat == DESTINATION_STATE) {
				// Destination reached

				signal AppReceive.receive(msg, payload, len);

				if (AUTO_ACK_BACK) {
					// if auto ack back is enabled, ack back
					saw->rout_stat = BROADCAST_STATE;
					saw->ACK = 1;
					call AppSendQueue.send(saw->srcNodeID, msg, len);
				}
			}
		}
		return msg;
	}

	event void BroadcastTimer.fired() {
		// If we have some packets to go out and we're not busy with one already...
		if (stored > 0 && !busy) {
			am_addr_t target;

			SprayAndWaitMsg_t* ptr = (SprayAndWaitMsg_t*)(call Packet.getPayload(&buffer[next], sizeof(SprayAndWaitMsg_t)));
			if (ptr->rout_stat == DESTINATION_STATE || available[next] <= 0) {
				target = ptr->destNodeID;
			} else {
				ptr->rout_stat = BROADCAST_STATE;
				target = AM_BROADCAST_ADDR;
			}

			if (call AMSend.send(target, &buffer[next], sizeof(SprayAndWaitMsg_t)) == SUCCESS) {
				call Leds.led0On();
				busy = TRUE;
				next++;
				if (next >= stored) {
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
			message_t* temp = memcpy(&buffer[stored], msg, sizeof(message_t));

			// prep the rest of the payload
			SprayAndWaitMsg_t* ptr = (SprayAndWaitMsg_t*)(call Packet.getPayload(temp, sizeof(SprayAndWaitMsg_t)));

			if (ptr == NULL) {
				return FAIL;
			}

			// TODO Make UUID actually useful with a random number or something
			ptr->uuid = 1;
			ptr->srcNodeID = TOS_NODE_ID;
			ptr->destNodeID = addr;
			ptr->rout_stat = BROADCAST_STATE;
			ptr->ACK = 0;

			// Update how many are stored int he queue
			available[stored] = SPRAY_WIDTH;
			cacheUUID[stored] = ptr->uuid;
			stored++;

			signal AppSendQueue.sendDone(msg, SUCCESS);

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
