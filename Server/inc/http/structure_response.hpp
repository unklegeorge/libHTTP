
#ifndef HTTP_RESPONSE
#define HTTP_RESPONSE

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <http/structure_base.hpp>

namespace http
{
namespace response
{

class structure :
	public structure_base
{
public:	
	enum
	{
		ok = 200,
		created = 201,
		accepted = 202,
		no_content = 204,
		multiple_choices = 300,
		moved_permanently = 301,
		moved_temporarily = 302,
		not_modified = 304,
		bad_request = 400,
		unauthorized = 401,
		forbidden = 403,
		not_found = 404,
		internal_server_error = 500,
		not_implemented = 501,
		bad_gateway = 502,
		service_unavailable = 503
	} status;
	
	structure(std::string status_code, std::string reason, std::string version) :
		status_code_(status_code),
		reason_(reason),
		version_("HTTP/"+version)
	{}
	virtual ~structure() {}
	
	virtual void to_buffers(std::vector<boost::asio::const_buffer>& v);
	
    const std::string& status_code() { return status_code_; }	
    const std::string& reason() { return reason_; }	
	const std::string& version() { return version_; }
	
protected:	
	// These are immutable after construction. the inherited headers are not.
	std::string status_code_;
	std::string reason_;
	std::string version_;

};

typedef boost::shared_ptr<structure> structure_ptr;

namespace reply
{

namespace status_strings {

const std::string ok = "OK\r\n";
const std::string created = "Created\r\n";
const std::string accepted = "Accepted\r\n";
const std::string no_content = "No Content\r\n";
const std::string multiple_choices = "Multiple Choices\r\n";
const std::string moved_permanently = "Moved Permanently\r\n";
const std::string moved_temporarily = "Moved Temporarily\r\n";
const std::string not_modified = "Not Modified\r\n";
const std::string bad_request = "Bad Request\r\n";
const std::string unauthorized = "Unauthorized\r\n";
const std::string forbidden = "Forbidden\r\n";
const std::string not_found = "Not Found\r\n";
const std::string internal_server_error = "Internal Server Error\r\n";
const std::string not_implemented = "Not Implemented\r\n";
const std::string bad_gateway = "Bad Gateway\r\n";
const std::string service_unavailable = "Service Unavailable\r\n";

std::string get_status(unsigned status);

} // namespace status_strings
} // namespace reply

} // namespace response

} // namespace http

#endif // HTTP_REQUEST
