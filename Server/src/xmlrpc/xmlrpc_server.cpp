
#include <http/xmlrpc/handler_request.hpp>
#include <http/xmlrpc/server.hpp>

namespace http
{
namespace server 
{

http::xmlrpc::procedure_manager& add_xmlrpc_handler(http::host& h, boost::filesystem::path uri)
{		
	request::xmlrpc::handler_ctor_ptr ctor(new request::xmlrpc::handler_ctor());
	h.add_handler(uri, request::handler_ctor_ptr(ctor));
	
	return ctor->manager();
}

http::xmlrpc::procedure_manager& add_xmlrpc_handler(http::host_ptr h, boost::filesystem::path uri)
{		
	return add_xmlrpc_handler(*h, uri);
}

} // namespace server
} // namespace http
