
#include "HAL.hpp"
#include "boiler.h"

HAL::HAL(int (*uart_write_fun)(char*),int(*bluetooth_write_fun)(char*)){
	uart_write_function = uart_write_fun;
	bt_write_function = bluetooth_write_fun;
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
