#include <PacketSerial.h>

#define DEBUG

PacketSerial myPacketSerial;

void setup() {

	myPacketSerial.begin(115200);
	myPacketSerial.setPacketHandler(&onPacketReceived);

	for (int i = 2; i < 54; i++) {
		pinMode(i, OUTPUT);
	}
}

void loop() {
	myPacketSerial.update();
}

void onPacketReceived(const uint8_t * buffer, size_t size) {
	char tempBuffer[size];
	memcpy(tempBuffer, buffer, size);
	uint8_t ackBuffer[size];
	memcpy(ackBuffer, buffer, size);
	String str(tempBuffer);

	//For the ack packet
	if (str.substring(0, 3) == "ack") {
		myPacketSerial.send(ackBuffer, size);
	}

////just debug
#ifdef DEBUG
	myPacketSerial.send(ackBuffer, size);
#endif

	//for the analogue pins we get a string with PWM at the start
	if (str.substring(0, 3) == "PWM") {
		String pinString = str.substring(str.indexOf('~') + 1, str.indexOf('/'));
		String valueString = str.substring(str.indexOf('/') + 1);
  analogWrite(pinString.toInt(), valueString.toInt());

////just debug
#ifdef DEBUG
    String debugString = "PWM pin: " + pinString + " value: " + valueString;
    char debugCharBuffer[debugString.length() + 1];
    uint8_t debugUintBuffer[debugString.length() + 1];

    for (int i = 0; i < debugString.length(); i++) {
      debugCharBuffer[i] = debugString[i];
    }
    debugCharBuffer[debugString.length()] = '\0';

    memcpy(debugUintBuffer, debugCharBuffer, debugString.length() + 1);
    myPacketSerial.send(debugUintBuffer, debugString.length() + 1);
#endif
	}

	///for the digital pins we get a string with DIG at the start
	if (str.substring(0, 3) == "DIG") {
		String pinString = str.substring(str.indexOf('~') + 1, str.indexOf('/'));
		String valueString = str.substring(str.indexOf('/') + 1);
		digitalWrite(pinString.toInt(), valueString.toInt());

////just debug
#ifdef DEBUG
		String debugString = "DIG pin: " + pinString + " value: " + valueString;
		char debugCharBuffer[debugString.length()];
		uint8_t debugUintBuffer[size];

		for (int i = 0; i > debugString.length(); i++) {
			debugCharBuffer[i] = debugString[i];
		}

		memcpy(debugUintBuffer, debugCharBuffer, debugString.length());
		myPacketSerial.send(debugUintBuffer, debugString.length());
#endif
	}
}
