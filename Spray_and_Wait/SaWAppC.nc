configuration SaWAppC {
}
implementation {
	components MainC;
	components SaWC;
	components AppC;
	components LedsC;
	components ActiveMessageC;
	components new TimerMilliC();

	SaWC.Boot -> MainC.Boot;
	SaWC.BroadcastTimer -> TimerMilliC;
	SaWC.AMControl -> ActiveMessageC;
	

	AppC.AMSend -> SaWC.AMSendQuene;
	AppC.Leds -> LedsC;
}
