#include "boiler.h"
void heater_on(){
	HAL_GPIO_WritePin(BOILERHEATERPORT, BOILERHEATERPIN, GPIO_PIN_RESET);
}
void heater_off(){
	HAL_GPIO_WritePin(BOILERHEATERPORT, BOILERHEATERPIN, GPIO_PIN_RESET);
}

int read_temperature(){
	return 0;
}

extern unsigned int blink_mode;

void led_blink_mode(unsigned int mode){
	if(mode>=3){
		return;
	}
	else{
		blink_mode = mode;
	}
}
