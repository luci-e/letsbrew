//#ifndef LB_PROTOCOL_INCLUDE
//#define LB_PROTOCOL_INCLUDE

#include <map>
#include <string>
#include <queue>
#include <list>
#include <cstdint>
#include <sstream>
#include <regex>
#include <stdio.h>
#include "lb_protocol.hpp"
#include "uart_support.h"

namespace letsbrew{

using namespace std;

	int lb_parse_all(const string &lb_request_string, lb_request &result){
//	    uint scanned;
//	    char cmd[16];
//	    uint duration;
//
//	    const char * c_string = lb_request_string.c_str();
//
//	    try{
//            scanned = sscanf(c_string, "%u %s %u\n", result.request_header.id, cmd, duration);
//            if(scanned != 3){
//                return PARSE_BAD_REQUEST;
//            }
//
//            string cmd_str = string(cmd);
//            if( cmd_str == "BREW" ){
//                result.request_header.CMD = BREW;
//            }else if(cmd_str == "STATE"){
//                result.request_header.CMD = STATE;
//            }else if(cmd_str == "KEEPWARM" ){
//                result.request_header.CMD = KEEPWARM;
//            }else{
//                return PARSE_BAD_REQUEST;
//            }
//
//            result.request_params["DURATION"] = to_string((int) duration);
//
//            return PARSE_OK;
//	    }catch(...){
//	        return PARSE_BAD_REQUEST;
//	    }
	    return PARSE_BAD_REQUEST;
	}


	LB_CMD string_to_lb_cmd( const string &str){
	    if( str == "BREW" ) return BREW;
        if( str == "STATE" ) return STATE;
        if( str == "KEEPWARM" ) return KEEPWARM;
        if( str == "CANCEL") return CANCEL;
        return NO_CMD;
	}

	/**
	 * Takes a well formed request and puts the result in an lb_request struct
	 * @param lb_request
	 * @param result
	 * @return
	 */
	int lb_parse_request( const string lb_request_string, lb_request &result ){
	    enum PARSER_STATE{
	        PARSE_HEADER,
	        PARSE_BODY,
	        PARSE_ERROR
	    }parser_state;

	    parser_state = PARSE_HEADER;

	    uint parser_line_header = 0;

	    regex header_reg[] = {
	            regex("\\s*\"ID\"\\s*:\\s*(\\d+)\\s*"),
                regex("\\s*\"USR\"\\s*:\\s*(\\d+)\\s*"),
                regex("\\s*\"TIME\"\\s*:\\s*(\\d+)\\s*"),
                regex("\\s*\"CMD\"\\s*:\\s*\"(.+)\"\\s*")
	    };

	    regex body_regex = regex("\\s*\"(.+)\"\\s*:\\s*(.+)\\s*");

	    smatch protocol_line;
	    stringstream request_stream = stringstream(lb_request_string);
	    string line;

	    while (getline(request_stream, line)) {
	        //PRINTF("%s\n", line.c_str());
	        switch(parser_state){

	            case PARSE_HEADER:{
	                if( regex_match( line, protocol_line, header_reg[parser_line_header]) ){
	                    try{
                            switch( parser_line_header){
                                case 0:
                                    result.request_header.id = stoul(protocol_line[1]);
                                    parser_line_header++;
                                    break;
                                case 1:
                                    result.request_header.usr = stoul(protocol_line[1]);
                                    parser_line_header++;
                                    break;
                                case 2:
                                    result.request_header.time = stoul(protocol_line[1]);
                                    parser_line_header++;
                                    break;
                                case 3:
                                    result.request_header.CMD = string_to_lb_cmd(protocol_line[1]);
                                    parser_line_header++;
                                    parser_state = PARSE_BODY;
                                    break;
                            }
	                    }catch(...){ return PARSE_BAD_REQUEST;}
	                }
	                break;
	            }

	            case PARSE_BODY:{
	                if( regex_match( line, protocol_line, body_regex) ){
	                    result.request_params[protocol_line[1]] = protocol_line[2];
	                }
	                break;
	            }

	        }
	    }

	    if( (parser_line_header == 4) && (result.request_header.CMD != NO_CMD) ){
	        return PARSE_OK;
	    }

		if( lb_parse_all(lb_request_string, result) == PARSE_OK ){
		    return PARSE_OK;
		}

		return PARSE_BAD_REQUEST;
	};

}
//#endif // LB_PROTOCOL_INCLUDE
