#include "boiler.h"
#include "adc.h"
#include <math.h>

#define NTCABOVEMIDPOINT
/*
 * if NTCABOVEMIDPOINT is defined then our circuit is like this:
 *
 * 							vcc
 * 							 |
 * 							NTC
 * 							 |
 * 							 + --- ADC
 * 							 |
 * 							 R
 * 							 |
 * 							GND
 *
 * if NTCABOVEMIDPOINT is not defined then the circuit is
 *
 * 							vcc
 * 							 |
 * 							 R
 * 							 |
 * 							 + --- ADC
 * 							 |
 * 							NTC
 * 							 |
 * 							GND
 */


void heater_on(){
	HAL_GPIO_WritePin(BOILERHEATERPORT, BOILERHEATERPIN, GPIO_PIN_RESET);
}
void heater_off(){
	HAL_GPIO_WritePin(BOILERHEATERPORT, BOILERHEATERPIN, GPIO_PIN_RESET);
}
//TODO find beta
int ntc_resistance_to_temperature_beta_equation(float ntc_resistance){
	const float beta = 0.1;
	const float t0 = 298.15;
	const float r0 = 1e5;
	const float r_inf = r0 * exp(-beta/t0);
	float temp = beta / (log(ntc_resistance/r_inf));
	return round(temp);
}

int ntc_resistance_to_temperature_linear(float ntc_resistance){
	const float resistance_points[2] = {100000,22111};// value at index zero is the "at rest" value 100k in our case
	const float temp_points[2] = {25,60}; //value at index 0 is the "at rest" temperature 25 deg. cent. in our case
	const float temp_range = temp_points[1] - temp_points[0];
	const float resistance_range = resistance_points[0]- resistance_points[1];

	float resistance_position = (resistance_points[0] - ntc_resistance)/resistance_range;

	// our current ntc value is resitance_position * resistance_range away from the "at rest" value
	float temperature = temp_points[0] + temp_range * resistance_position;
	return round(temperature);
}
//TODO choose conversion function
int read_temperature(){
	unsigned int reading = adc2_read();
	const float fixed_resistor_value = 50000.0;
	float ntc_resistance;
	const float max_possible_adc_reading = 4098.0;
	float inverse_ratio = max_possible_adc_reading/(float)reading;

#ifdef NTCABOVEMIDPOINT
	ntc_resistance = fixed_resistor_value * ( inverse_ratio -1 );

#else
	ntc_resistance = fixed_resistor_value * (1 / (inverse_ratio - 1));

#endif
	return ntc_resistance_to_temperature(ntc_resistance);
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
