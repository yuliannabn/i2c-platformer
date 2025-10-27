#include <Arduino.h>
#include <Wire.h>

#include "joystick.h"


#define PIN_VRY				A1
#define PIN_VRX				A0
#define PIN_SW				13

uint8_t rxCmd;
bool read;

joystick_t data { 0, 0, false };

void ReceiveFn(int bytes_read) {
	if (bytes_read > 0) {
		rxCmd = Wire.read();
		switch (rxCmd) {
			case CMD_READ_JOYSTICK_INFO:
				data.x = analogRead(PIN_VRX);
				data.y = analogRead(PIN_VRY);
				data.button = !digitalRead(PIN_SW);
				read = true;
				break;
			default:
				read = false;
				break;
		}
	}
}

void TransferFn() {
	if (read)
		Wire.write((uint8_t*)&data, sizeof(data));
}

void setup() {
	Serial.begin(9600);

	Wire.begin(SLAVE_ADDR);
	Wire.onReceive(ReceiveFn);
	Wire.onRequest(TransferFn);
	Serial.println("Slave initialization complete.");

	pinMode(PIN_VRY, INPUT);
	delay(600);
	pinMode(PIN_VRX, INPUT);
	delay(600);
	pinMode(PIN_SW, INPUT);
	delay(600);
	Serial.println("Joystick pins initialization complete.");
}

void loop() {
	Serial.println(read);
	delay(600);
}
