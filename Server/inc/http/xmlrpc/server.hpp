
#ifndef HTTP_SERVER_XMLRPC
#define HTTP_SERVER_XMLRPC

#include <http/server.hpp>
#include <http/xmlrpc/xmlrpc.hpp>

namespace http
{
namespace server 
{

http::xmlrpc::procedure_manager& add_xmlrpc_handler(http::host& h, boost::filesystem::path uri);

http::xmlrpc::procedure_manager& add_xmlrpc_handler(http::host_ptr h, boost::filesystem::path uri);

} // namespace server
} // namespace http

#endif // HTTP_SERVER_XMLRPC
