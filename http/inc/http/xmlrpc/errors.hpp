#ifndef HTTP_XMLRPC_ERRORS
#define HTTP_XMLRPC_ERRORS

namespace http
{
namespace xmlrpc
{

struct failure
{
	enum
	{
		xml_invalid = 10100,
		xml_unknown_type,
		xml_bad_int,
		xml_bad_string,
		xml_bad_double,
		xml_bad_base64,
		xml_bad_datetime,
		xml_bad_array,
		xml_bad_struct,
		xml_empty_node,
		xml_invalid_node,
		xml_missing_fault_code,
		xml_missing_fault_string,
		
		xml_exception_caught = 10200,
		exception_caught,
		tinyxml_parse_error,
		bad_response,
		bad_request_parameter,
		bad_request_method_name,
		no_matching_method,
		
		server_code_offset = 10000
		
	};
};


} // namespace xmlrpc
} // namespace http

#endif // HTTP_XMLRPC_ERRORS
