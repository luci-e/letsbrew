
#include "HAL.hpp"
#include "boiler.h"


void HAL::stop_heater(){
	heater_off();
}
void HAL::start_heater(){
	heater_on();

}
int HAL::get_temperature(){
	return read_temperature();
}

void HAL::set_blink_mode(unsigned int mode){

	led_blink_mode(mode);
}
