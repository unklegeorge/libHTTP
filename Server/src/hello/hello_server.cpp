
#include <http/hello/server.hpp>
#include <http/hello/handler_request_hello.hpp>
#include <http/hello/handler_request_hello_xml.hpp>

namespace http
{
namespace server 
{

void add_hello_handler(http::host& h, boost::filesystem::path uri)
{		
	h.add_handler(uri, request::handler_ctor_ptr(new request::handler_hello_ctor()));
}

void add_hello_handler(http::host_ptr h, boost::filesystem::path uri)
{		
	add_hello_handler(*h, uri);
}

void add_hello_xml_handler(http::host_ptr h, boost::filesystem::path uri)
{		
	h->add_handler(uri, request::handler_ctor_ptr(new request::handler_hello_xml_ctor()));
}

} // namespace server
} // namespace http
