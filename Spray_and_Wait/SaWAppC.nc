#include "saw.h"

configuration SaWAppC {
}
implementation {
	components MainC;
	components SaWC;
	components AppC;
	components LedsC;
	components ActiveMessageC;
	components new TimerMilliC();
	components new AMSenderC(AM_SPRAYNWAIT);
  components new AMReceiverC(AM_SPRAYNWAIT);

	SaWC.Boot -> MainC.Boot;
	SaWC.BroadcastTimer -> TimerMilliC;
	SaWC.AMControl -> ActiveMessageC;
	SaWC.Leds -> LedsC;
	SaWC.AMSend -> AMSenderC;
	SaWC.Receive -> AMReceiverC;

	AppC.AMSend -> SaWC.AppSendQueue;
	AppC.Leds -> LedsC;
	AppC.Receive -> SaWC.AppReceive;
}
