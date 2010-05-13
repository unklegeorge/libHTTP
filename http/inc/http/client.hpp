
#ifndef HTTP_CLIENT
#define HTTP_CLIENT

#include <http/client_connection_base.hpp>
#include <http/client_connection.hpp>
#if defined(HTTP_SUPPORT_OPENSSL)
#	include <http/client_ssl_connection.hpp>
#endif
#include <http/client_verb.hpp>
#include <http/client_verb_post.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace client 
{

class error : public std::exception {
public:
	error(const std::string& message) :
		str_(message)
	{}
	virtual ~error() throw () {}
	
	virtual const char *what() const throw () 
	{
		return (str_.c_str());
	}	
	
private:
	std::string str_;
};

} // namespace server
} // namespace http

#endif // HTTP_CLIENT
