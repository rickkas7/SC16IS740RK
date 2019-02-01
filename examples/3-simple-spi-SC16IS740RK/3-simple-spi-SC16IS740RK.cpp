#include "SC16IS740RK.h"

// Pick a debug level from one of these two:
SerialLogHandler logHandler;
// SerialLogHandler logHandler(LOG_LEVEL_TRACE);

//SC16IS740 extSerial(Wire, 0);
SC16IS740SPI extSerial(SPI, A2);

char out = ' ';

void setup() {
	Serial.begin(9600);

	delay(5000);

	extSerial.begin(9600);
}

void loop() {
	while(extSerial.available()) {
		int c = extSerial.read();
		Log.info("received %d", c);
	}

	extSerial.print(out);
	if (++out >= 127) {
		out = ' ';
	}
	delay(100);
}
