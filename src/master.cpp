#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal.h>

#include "joystick.h"

#define X_AXIS 'x'
#define Y_AXIS 'y'

#define AXIS_LIMIT 1023
#define AXIS_NEUTRAL 512

#define EMPTY '_'
#define OBSTACLE 'O'
#define PLAYER 'x'

#define PLAYER_X_POS 1
#define DISPLAY_WIDTH 16
#define DISPLAY_HEIGHT 2

#define TOP		0
#define BOTTOM 	1

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

char display_frame[DISPLAY_HEIGHT][DISPLAY_WIDTH];

char frame_map[DISPLAY_HEIGHT][(DISPLAY_WIDTH*2)+1] = { 0 };

bool has_surrounding_obstacles(int index, int row)
{
	if ((index - 1 < 0) || (index + 1) >= DISPLAY_WIDTH)
		return (true);

	for (int a = index - 1; a <= (index + 1); a++)
		if (frame_map[row][a] == OBSTACLE)
			return (true);

	return (false);
}

void generate_tile(int index, int row)
{
	switch (row) {
		case TOP:
		{
			bool make_tile = (!has_surrounding_obstacles(index, BOTTOM)) && (random(200) % 2 == 0);
			frame_map[TOP][index] = (make_tile) ? OBSTACLE : EMPTY;
		}
		case BOTTOM:
			frame_map[BOTTOM][index] = (random(200) % 6 == 0) ? OBSTACLE : EMPTY;
		default:
			break;
	}
}

void generate_map(int map_curr) {
	size_t limit = (map_curr == DISPLAY_WIDTH) ? DISPLAY_WIDTH : DISPLAY_WIDTH*2;
	size_t start = limit - DISPLAY_WIDTH;

	for (size_t a = start; a < limit; a++)
		generate_tile(a, BOTTOM);
	for (size_t a = start; a < limit; a++)
		generate_tile(a, TOP);

	Serial.println(frame_map[TOP]);
	Serial.println(frame_map[BOTTOM]);
}

void display_map_frame() {
	static int map_curr = 0;

	if (map_curr == DISPLAY_WIDTH*2)
		map_curr = 0;

	if (map_curr % DISPLAY_WIDTH == 0)
		generate_map(map_curr);

	for (int row = TOP; row <= BOTTOM; row++) {
		lcd.setCursor(0, row);

		strncpy(display_frame[row], frame_map[row] + map_curr, DISPLAY_WIDTH);
		int len = strlen(display_frame[row]);
		if (len < DISPLAY_WIDTH)
			strncpy(display_frame[row] + len, frame_map[row], DISPLAY_WIDTH - len);

		lcd.print(display_frame[row]);
	}
	map_curr++;
}

int display_player_frame(int y_val) {
	int y_pos = (y_val > AXIS_NEUTRAL);
	static int score = 1;

	lcd.setCursor(PLAYER_X_POS, y_pos);
	if (display_frame[y_pos][PLAYER_X_POS] == OBSTACLE) {
		lcd.clear();
		lcd.print("___ you lost ___");
		lcd.setCursor(1, 3);
		lcd.print("score: ");
		lcd.print(score);
		score = 0;
		delay(1000);
	}
	score++;
	lcd.write(PLAYER);
	return (score);
}

void setup() {
	Serial.begin(9600);
	lcd.begin(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	Serial.println("LCD initialization complete.");
	delay(20);
	Wire.begin();
	Serial.println("Master initialization complete.");
	delay(20);
	generate_map(DISPLAY_WIDTH);
	generate_map(0);
}

void loop() {
	joystick_t data { 0, 0, false };
	lcd.clear();

	delay(20);

	Wire.beginTransmission(SLAVE_ADDR);
	Wire.write(CMD_READ_JOYSTICK_INFO);
	Wire.endTransmission();

	delay(20);

	Wire.requestFrom(SLAVE_ADDR, sizeof(data));
	if (Wire.available() == sizeof(data)) {
		Wire.readBytes((uint8_t*)&data, sizeof(data));
		display_map_frame();
		display_player_frame(data.y);
	}
	delay(300);
}
