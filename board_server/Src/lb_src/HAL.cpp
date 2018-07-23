
#include "HAL.hpp"
#include "boiler.h"

HAL::HAL(void (*uwf)(char*),void(*bwf)(char*)){
	uart_write_function = uwf;
	bt_write_function = bwf;
}

void HAL::stop_heater(){
	heater_off();
}
void HAL::start_heater(){
	heater_on();

}

void HAL::write_on_uart(char *str){
	uart_write_function(str);
}
void HAL::write_on_bluetooth(char *str){
	bt_write_function(str);
}

int HAL::get_temperature(){
	return read_temperature();
}

void HAL::set_blink_mode(unsigned int mode){

	led_blink_mode(mode);
}
