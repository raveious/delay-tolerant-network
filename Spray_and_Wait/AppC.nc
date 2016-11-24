module AppC {
	uses {
		interface Leds;
		interface Boot;
		interface AMSend;
	}
}
implementation {
	event void Boot.booted() {
	}

	event void AMSend.sendDone(message_t* msg, error_t err) {
		// TODO Only decrement the available copies
	}
}
