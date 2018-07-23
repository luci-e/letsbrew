#include "boiler.h"
#include "adc.h"
void heater_on(){
	HAL_GPIO_WritePin(BOILERHEATERPORT, BOILERHEATERPIN, GPIO_PIN_RESET);
}
void heater_off(){
	HAL_GPIO_WritePin(BOILERHEATERPORT, BOILERHEATERPIN, GPIO_PIN_RESET);
}

int read_temperature(){
	unsigned int reading = adc2_read();
	return (reading/1023)+20;
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
