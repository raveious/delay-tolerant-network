configuration BinSaWAppC {
}
implementation {
	components MainC;
	components BinSaWC;
	components AppC;
	components LedsC;
	components ActiveMessageC;
	components new TimerMilliC();

	BinSaWC.Boot -> MainC.Boot;
	BinSaWC.BroadcastTimer -> TimerMilliC;
	BinSaWC.AMControl -> ActiveMessageC;
	

	AppC.AMSend -> BinSaWC.AMSendQueue;
	AppC.Leds -> LedsC;
}
