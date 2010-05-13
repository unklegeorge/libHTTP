#ifndef HTTP_CLIENT_XMLRPC
#define HTTP_CLIENT_XMLRPC

#include <boost/variant.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/format.hpp>

#include <tinyxml/tinyxml.h>

#include <http/client.hpp>
#include <http/xmlrpc/handler_response.hpp>
#include <http/xmlrpc/xmlrpc.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace client 
{
namespace xmlrpc
{
	
http::xmlrpc::param call(http::client::sync::connection_base_ptr con, 
	const boost::filesystem::path& uri, const std::string& method_name, 
	const http::xmlrpc::param_arguments& params, bool keep_alive = false);

} // namespace xmlrpc
} // namespace client
} // namespace http

#endif // HTTP_CLIENT_XMLRPC
