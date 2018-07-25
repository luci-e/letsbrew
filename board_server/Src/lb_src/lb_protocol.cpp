//#ifndef LB_PROTOCOL_INCLUDE
//#define LB_PROTOCOL_INCLUDE

#include <map>
#include <string>
#include <queue>
#include <list>
#include <cstdint>
#include <sstream>
#include <regex>
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
		static regex header_reg[5] = {
			regex( "^ *ID *: *(\\d{1,10}) *"),
			regex( "^ *USR *: *(\\d{1,10}) *" ),
			regex( "^ *TIME *: *(\\d{1,10}) *" ),
            regex( "^ *CMD *: *([A-z]+) *" ),
            regex( "^")
		};
		
		stringstream ss( lb_header );
		int field = 0;

		string line;
		smatch matches;
		string value;

		while ( getline( ss, line ) ) {
			if ( regex_search( line, matches, header_reg[field] ) ) {
				value = matches[1];

				switch ( field ) {
					case 0:
					{
						result.id = stoi( value );
						break;
					}


					case 1:
					{
						result.usr = stoi( value );
						break;
					}

					case 2:
					{
						result.time = stoi( value );
						break;
					}

                    case 3:
                    {
                        if ( value == "BREW" ) {
                            result.CMD = BREW;
                        } else if ( value == "CANCEL" ) {
                            result.CMD = CANCEL;
                        } else if ( value == "STATE" ) {
                            result.CMD = STATE;
                        } else if ( value == "KEEPWARM" ) {
                            result.CMD = KEEPWARM;
                        } else {
                            return PARSE_BAD_HEADER;
                        }

                        break;
                    }

                    case 4:
                    {
                        // Hello there
                        break;
                    }

					default:
					{
						return PARSE_BAD_HEADER;
					}
				}


			} else {
				return PARSE_BAD_HEADER;
			}
			
			field++;
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
		static regex field_reg( "^ *([A-z_\\d]+) *: *([A-z\\d]+) *" );

		stringstream ss( lb_body );

		string line;
		smatch matches;

		while ( getline( ss, line ) ) {
		    if( line.empty() ){
                if ( regex_search( line, matches, field_reg ) ) {
                    result.request_params[matches[1]] = matches[2];
                } else {
                    return PARSE_BAD_BODY;
                }
		    }
		}

		return PARSE_OK;
	};

	int lb_check_request( const lb_request &request ) {

		map<string, string>::const_iterator it;
		const map<string, string> &mapref = request.request_params;

		switch ( request.request_header.CMD ) {
			case BREW:
			{
				/* Check time present h20 temp limit check H20 amount */
				it = request.request_params.find( "EXEC_TIME" );
				if ( it == mapref.end() ) {
					return PARSE_BAD_BODY;
				}

				it = request.request_params.find( "H2O_TEMP" );
				if ( it == mapref.end() ) {
					return PARSE_BAD_BODY;
				}

				it = request.request_params.find( "H2O_AMOUNT" );
				if ( it == mapref.end() ) {
					return PARSE_BAD_BODY;
				}

				break;
			}
			
			case STATE:
			{
				/* I'm the ghost of simmetry keeping this here for completeness */
				break;
			}

			case CANCEL:
			{
				/* Maybe later */
				break;
			}

			case KEEPWARM:
			{
				it = request.request_params.find( "DURATION" );
				if ( it == mapref.end() ) {
					return PARSE_BAD_BODY;
				}

				break;
			}

			default:
				break;
		}

		return PARSE_OK;
	};

	/**
	 * Takes a well formed request and puts the result in an lb_request struct
	 * @param lb_request
	 * @param result
	 * @return
	 */
	int lb_parse_request( const string &lb_request_string, lb_request &result ){
		string header_string;
		string body_string;

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

		if ( lb_check_request( result ) != PARSE_OK ) {
			return PARSE_BAD_BODY;
		}

		return PARSE_OK;
	};

}
//#endif // LB_PROTOCOL_INCLUDE
