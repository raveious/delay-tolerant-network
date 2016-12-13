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
		// TODO send data to the destination
	}

	event void AMSend.sendDone(message_t* msg, error_t err) {
	}

	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len) {
		// TODO Send ACK back

		// denote that a packet was received
		call Leds.led2On();
	}
}
