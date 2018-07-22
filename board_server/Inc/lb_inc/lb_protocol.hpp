#pragma once

#ifndef LB_PROTOCOL_INCLUDE
#define LB_PROTOCOL_INCLUDE

#include <map>
#include <string>
#include <queue>
#include <list>
#include <cstdint>
#include <sstream>
#include <regex>

namespace letsbrew{

using namespace std;

#define MAX_MESSAGE_SIZE 512

	enum LB_CMD{
		BREW,
		CANCEL,
		STATE,
		KEEPWARM
	};

	enum PARSE_CODE{
		PARSE_OK,
		PARSE_BAD_HEADER,
		PARSE_BAD_BODY
	};

	struct lb_request_header{
		uint32_t id;
		uint32_t usr;
		LB_CMD CMD;
		uint32_t time;
	};

	struct lb_request{
		lb_request_header request_header;
		map<string, string> request_params;
	};


	struct lb_response_header{
		uint32_t id;
		uint32_t server;
		LB_CMD CMD;
		uint32_t time;
	};

	struct lb_response {
		lb_response_header response_header;
		map<string, string> response_params;
	};

//	std::string& ltrim( std::string& str, const std::string& chars = "\t\n\v\f\r " ) {
//		str.erase( 0, str.find_first_not_of( chars ) );
//		return str;
//	}
//
//	std::string& rtrim( std::string& str, const std::string& chars = "\t\n\v\f\r " ) {
//		str.erase( str.find_last_not_of( chars ) + 1 );
//		return str;
//	}
//
//	std::string& trim( std::string& str, const std::string& chars = "\t\n\v\f\r " ) {
//		return ltrim( rtrim( str, chars ), chars );
//	}

	/**
	 * Takes a well formed request_header and puts the result in the result struct
	 * @param lb_request_header
	 * @param result
	 * @return
	 */
	int lb_parse_header( const string &lb_header, lb_request_header &result ) ;

	/**
	* Takes a well formed request_header and puts the result in the result struct
	* @param lb_body
	* @param result
	* @return
	*/
	int lb_parse_body( const string &lb_body, lb_request &result );
	int lb_check_request( const lb_request &request ) ;

	/**
	 * Takes a well formed request and puts the result in an lb_request struct
	 * @param lb_request
	 * @param result
	 * @return
	 */
	int lb_parse_request( const string &lb_request_string, lb_request &result );
}
#endif // LB_PROTOCOL_INCLUDE
