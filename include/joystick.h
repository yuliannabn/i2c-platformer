#include <Arduino.h>

#pragma once

#define SLAVE_ADDR 0x69

#define CMD_READ_JOYSTICK_INFO 0x47

typedef struct __attribute__((packed)) JoystickInfo {
	int x;
	int y;
	bool button;
} joystick_t;
