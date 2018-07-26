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
namespace letsbrew{

using namespace std;

	std::string& ltrim( std::string& str, const std::string& chars = "\t\n\v\f\r " ) {
		str.erase( 0, str.find_first_not_of( chars ) );
		return str;
	}

	std::string& rtrim( std::string& str, const std::string& chars = "\t\n\v\f\r " ) {
		str.erase( str.find_last_not_of( chars ) + 1 );
		return str;
	}

	std::string& trim( std::string& str, const std::string& chars = "\t\n\v\f\r " ) {
		return ltrim( rtrim( str, chars ), chars );
	}

	/**
	 * Takes a well formed request_header and puts the result in the result struct
	 * @param lb_request_header
	 * @param result
	 * @return
	 */
	int lb_parse_header( const string &lb_header, lb_request_header &result ) {
	    char cmd_string[10];
	    uint scanned = 0;

	    scanned = sscanf(lb_header.c_str(), "ID:%u\nUSR:%u\nTIME:%u\nCMD:%s\n", &result.id, &result.usr, &result.time, cmd_string);
	    if( scanned != 4 ){
	        return PARSE_BAD_HEADER;
	    }

	    string cmd_str = string(cmd_string);

	    if( cmd_str == "BREW" ){
	        result.CMD = BREW;
	    }else if( cmd_str == "STATE"){
	        result.CMD = STATE;
	    }else if( cmd_str == "KEEPWARM"){
	        result.CMD = KEEPWARM;
        }else{
            return PARSE_BAD_HEADER;
        }

	    return PARSE_OK;
	};

	/**
	* Takes a well formed request_header and puts the result in the result struct
	* @param lb_body
	* @param result
	* @return
	*/
	int lb_parse_body( const string &lb_body, lb_request &result ) {
	    uint scanned;

		switch( result.request_header.CMD ){
		    case BREW:{
		        char exec_time[12], h2o_amount[12], h2o_temp[12];
		        scanned = sscanf(lb_body.c_str(), "EXEC_TIME:%s\nH2O_TEMP:%s\nH2O_AMOUNT:%s\n", exec_time, h2o_amount, h2o_temp );
		        if( scanned != 3 ){
		            return PARSE_BAD_HEADER;
		        }

                result.request_params["EXEC_TIME"] = string(exec_time);
                result.request_params["H2O_AMOUNT"] = string(h2o_amount);
		        result.request_params["H2O_TEMP"] = string(h2o_temp);
		        break;
		    }

		    case STATE:{
		        return PARSE_OK;
		    }

		    case KEEPWARM:{
                char duration[12];
                scanned = sscanf(lb_body.c_str(), "DURATION:%s\n", duration);
                if( scanned != 1 ){
                    return PARSE_BAD_HEADER;
                }

                result.request_params["DURATION"] = string(duration);
                break;
		    }

		    default:
		        return PARSE_BAD_HEADER;
		}

		return PARSE_OK;
	};


	int lb_parse_all(const string &lb_request_string, lb_request &result){
	    uint scanned;
	    char cmd[16];
	    uint duration;

	    const char * c_string = lb_request_string.c_str();

	    scanned = sscanf(c_string, "%u %s %u\n", result.request_header.id, cmd, duration);
	    if(scanned != 3){
	        return PARSE_BAD_REQUEST;
	    }

	    string cmd_str = string(cmd);
	    if( cmd_str == "BREW" ){
	        result.request_header.CMD = BREW;
	    }else if(cmd_str == "STATE"){
            result.request_header.CMD = STATE;
	    }else if(cmd_str == "KEEPWARM" ){
            result.request_header.CMD = KEEPWARM;
	    }else{
	        return PARSE_BAD_REQUEST;
	    }

        result.request_params["DURATION"] = to_string((int) duration);

        return PARSE_OK;
	}


	/**
	 * Takes a well formed request and puts the result in an lb_request struct
	 * @param lb_request
	 * @param result
	 * @return
	 */
	int lb_parse_request( const string &lb_request_string, lb_request &result ){
		string header_string;
		string body_string;

		if( lb_parse_all(lb_request_string, result) == PARSE_OK ){
		    return PARSE_OK;
		}

		// Find the header
		size_t head_end = lb_request_string.find( "\r\n" );
		if( head_end == string::npos ){
			return PARSE_BAD_HEADER;
		} else {
			header_string = lb_request_string.substr( 0, head_end );
		}

		body_string = lb_request_string.substr( head_end + 2, string::npos );

		if ( lb_parse_header( header_string, result.request_header ) != PARSE_OK ) {
			return PARSE_BAD_HEADER;
		}

		if ( lb_parse_body( body_string, result ) != PARSE_OK ) {
			return PARSE_BAD_BODY;
		}

		return PARSE_OK;
	};

}
//#endif // LB_PROTOCOL_INCLUDE
