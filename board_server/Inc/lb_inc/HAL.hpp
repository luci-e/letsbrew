
#include "stm32f4xx_hal.h"

class HAL{
private:
	void (*uart_write_function)(char*);
	void (*bt_write_function)(char*);
    public:
		HAL(void (*uwf)(char*),void(*bwf)(char*));
        void stop_heater();
        void start_heater();
   
        int get_temperature();
        void set_blink_mode(unsigned int mode);
        void write_on_uart(char * str);
        void write_on_bluetooth(char * str);
};

