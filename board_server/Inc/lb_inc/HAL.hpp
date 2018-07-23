
#include "stm32f4xx_hal.h"

class HAL{
private:
	int (*uart_write_function)(char*);
	int (*bt_write_function)(char*);
    public:
		HAL(int (*uart_write_fun)(char*),int(*bluetooth_write_fun)(char*));
        void stop_heater();
        void start_heater();
   
        int get_temperature();
        void set_blink_mode(unsigned int mode);
        void write_on_uart(char * str);
        void write_on_bluetooth(char * str);
};

