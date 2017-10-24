#include <init.h>
#include <constants.h>
#include <ucos.h>
#include <utils.h>
#include <string.h>
#include <fdprintf.h>
#include <system.h>

#include "mqtt-paho/MQTTClient.h"
#include "mqtt-paho/NBMQTTSocket.h"
#include "mqtt-paho/NBMQTTCountdown.h"

extern "C" {
void UserMain(void * pd);
}

int arrivedcount = 0;


const char * AppName="MQTT-Nano";

void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    reinterpret_cast<char *>(message.payload)[message.payloadlen] = 0;
	iprintf("Message %d arrived: qos %d, retained %d, dup %d, packetid %d\r\nPayload: %s\r\n",
		++arrivedcount, message.qos, message.retained, message.dup, message.id,(char *)message.payload);
}

void UserMain(void * pd) {
	init();
    iprintf("Application started\n");

    char name[32] = { 0 };
	siprintf(name, "nb-%02X%02X",gConfigRec.mac_address[4], gConfigRec.mac_address[5]); // Name device "nb-(last 2 bytes of mac address)

	NBMQTTSocket mqttSocket;

    MQTT::Client<NBMQTTSocket,NBMQTTCountdown> client = MQTT::Client<NBMQTTSocket,NBMQTTCountdown>(mqttSocket);

    MQTTPacket_connectData connectOptions = MQTTPacket_connectData_initializer;
	connectOptions.MQTTVersion = 3;
	connectOptions.keepAliveInterval = 60; // We must send a keep-alive within this interval
	connectOptions.cleansession = 1;
	connectOptions.clientID.cstring = name;

	while (1) {
		bool sub = false;
		if (client.isConnected()) {
			if (!sub) {
				client.subscribe("nbuptime", MQTT::QOS0, messageArrived);
				sub = true;
			}
		    MQTT::Message message;
		    char buf[100] = { 0 };
		    siprintf(buf, "%s -> Uptime: %lu", name, Secs);
		    message.qos = MQTT::QOS0;
		    message.retained = false;
		    message.dup = false;
		    message.payload = (void*)buf;
		    message.payloadlen = strlen(buf)+1;

		    client.publish("nbuptime",message);
		    client.yield(100); // Issue keep-alive
		} else {
			if (mqttSocket.mysock < 0) { // is TCP socket open?
				// open socket to AWS
				while (mqttSocket.connect("iot.eclipse.org",1883) != 0) {
					// connection failure, keep trying
					iprintf("Connection failure, retrying...\r\n");
					OSTimeDly(TICKS_PER_SECOND * 15);
				}
			}
			// Initiate MQTT connection over TCP socket
			client.connect(connectOptions);
		}
		OSTimeDly(TICKS_PER_SECOND * 30);
	}
}
