

#include <iostream>
#include "controller.hpp"


#include <string>

#include "lb_protocol.hpp"


#define TIMETOBREW 60
#define BREWSTARTTEMP 80
#define BREWMAXTOSTART 50
#define KWKEEPABOVE 60
#define KWKEEPBELOW 65
#define KWMINTOSTART 50

#define seconds_to_ticks(x) x * 1

using namespace std;

using namespace letsbrew;


Controller::Controller(HAL * usehal){
	hal = usehal;
    state = IDLE;
    last_error = NOERROR;
    ticks_to_go = 0;
}

char * Controller::last_err_to_str(){

	return err_to_str(last_error);

}

char * Controller::err_to_str(AUTOMERRORS err){
    switch(err){
        case NOERROR:
            return "No error";
        case BREWINPROGRESS:
            return "Brewing already in progress";
        case BREWTEMPERATURETOOHIGH:
            return "Unable to start brewing, heater too hot!";
        case HEATERERROR:
            return "Heater error";
        case KEEPWARMTOOCOLD:
            return "Unable to keep-warm, too cold";
        case KEEPWARMINPROGRESS:
            return "Keep-warm already in progress";
        default:
        	return "";
        	break;

    }
}

void Controller::parse(unsigned int channel,char new_character){

//	for ( auto c : good_request ) {
//		auto ret = sr.push_char( c );
//
//		switch ( ret ) {
//			case STREAM_CODES::STREAM_OUT_OF_BOUND:
//			{
//				cout << "Stream out of bound " << endl;
//				goto out;
//			}
//
//			case STREAM_CODES::STREAM_DONE:
//			{
//				cout << "Stream done " << endl;
//				goto out;
//			}
//
//		}
//	}out:

	if(channel>=NUMCHANNELS){
		return;
	}
	char * retMesg;

	channels[channel].push_char(new_character);
	if(channels[channel].message_complete){
		letsbrew::lb_request lbr;
		auto result =letsbrew::lb_parse_request( channels[channel].message_buffer, lbr );
		if(result == PARSE_OK){
			switch(lbr.request_header.CMD){
			case BREW:
			{
					auto er = brew();
					retMesg = err_to_str(er);
					break;
			}
			case CANCEL:
			{
					abort();
					retMesg = last_err_to_str();
					break;
			}
			case(STATE):
			{
					retMesg = last_err_to_str();
					break;
			}
			case(KEEPWARM):
			{
					unsigned int duration = stoi(lbr.request_params["DURAITON"]);
					auto er=keep_warm(duration);
					retMesg = err_to_str(er);
					break;
			}

			}
		}
		respond(channel,retMesg);
	}




}


void Controller::respond(unsigned int channel, char * msg){
	;;;;//tudu tudu tudududududududu (running in the 90s)
}

char * Controller::state_to_str(){
    switch(state){
        case IDLE:
            return "IDLE";
            break;
        case BREWPREHEATING:
            return "Brewing preheating";
            break;
        case BREWBREWING:
            return "Brewing";
            break;
        case KEEPWARMWARMING:
            return "Keep-warm active";
            break;
        case KEEPWARMIDLE:
            return "Keep-warm idle";
            break;
        case ERR:
            return "Error state";
            break;
        default:return ""; break;

    }
}



AUTOMERRORS Controller::abort(){
    hal->stop_heater();
    state = IDLE;
    last_error = NOERROR;
    ticks_to_go = 0;
    return last_error;
}
AUTOMERRORS Controller::keep_warm(unsigned int seconds){
    switch(state){
        case IDLE:
            if(hal->get_temperature() < KWMINTOSTART){
                last_error = KEEPWARMTOOCOLD ;
            }
            else{
                state = KEEPWARMIDLE;
                last_error =NOERROR;
                ticks_to_go = seconds_to_ticks(seconds);
            }
            break;
        case BREWPREHEATING:
            last_error = BREWINPROGRESS;
            break;
        case BREWBREWING:
            last_error = BREWINPROGRESS;
            break;
        case KEEPWARMWARMING:
            last_error = KEEPWARMINPROGRESS;
            break;
        case KEEPWARMIDLE:
            last_error = KEEPWARMINPROGRESS;
            break;
        case ERR:
            break;
        default:

        	break ;

    }
    return last_error;
}
AUTOMERRORS Controller::brew(){
    switch(state){
        case IDLE:
            if(hal->get_temperature() > BREWMAXTOSTART){
                last_error =BREWTEMPERATURETOOHIGH;
            }
            else{
                state =BREWPREHEATING;
                last_error =NOERROR;
            }
            break;
        case BREWPREHEATING:
            last_error = BREWINPROGRESS;
            break;
        case BREWBREWING:
            last_error = BREWINPROGRESS;
            break;
        case KEEPWARMWARMING:
            last_error = KEEPWARMINPROGRESS;
            break;
        case KEEPWARMIDLE:
            last_error = KEEPWARMINPROGRESS;
            break;
        case ERR:
            break;
        default: break;

    }
    return last_error;
}

void Controller::tick(){
    switch(state){
        case IDLE:
        	hal->set_blink_mode(0);
            return;
            break;
        case BREWPREHEATING:
        	hal->set_blink_mode(1);
            if(hal->get_temperature() > BREWSTARTTEMP){
                state = BREWBREWING;
                hal->start_heater();
                ticks_to_go = seconds_to_ticks(TIMETOBREW);
            }
            break;
        case BREWBREWING:
        	hal->set_blink_mode(1);
            ticks_to_go-=1;
            if(ticks_to_go == 0){
                last_error = NOERROR;
                hal->stop_heater();
                state = IDLE;
            }
            break;
        case KEEPWARMWARMING:
        	hal->set_blink_mode(1);
            ticks_to_go-=1;
            if(ticks_to_go == 0){
                last_error = NOERROR;
                hal->stop_heater();
                state = IDLE;
            }
            if(hal->get_temperature()> KWKEEPBELOW){
                hal->stop_heater();
                state = KEEPWARMIDLE;
            }

            break;
        case KEEPWARMIDLE:
        	hal->set_blink_mode(1);
            ticks_to_go-=1;
            if(ticks_to_go == 0){
                last_error = NOERROR;
                hal->stop_heater();
                state = IDLE;
            }
            if(hal->get_temperature()<KWKEEPABOVE){
                hal->start_heater();
                state = KEEPWARMWARMING;
            }
            break;
        case ERR:
        	hal->set_blink_mode(2);
            break;
        default: break;

    }

}


