
#ifndef HTTP_CLIENT_HELLO
#define HTTP_CLIENT_HELLO

#include <http/client.hpp>

namespace http
{
namespace client 
{

// All the logic for a handler is contained in two files. The first which implements 
// the handler itself and a second similar to this which describes the interface for 
// the client application. *** P.S. This need exception catches ***

std::string hello(http::client::sync::connection_base_ptr con, 
	const boost::filesystem::path& uri, std::string name, bool keep_alive);

std::string hello_xml(http::client::sync::connection_base_ptr con, 
	const boost::filesystem::path& uri, std::string name, bool keep_alive);

} // namespace client
} // namespace http

#endif // HTTP_CLIENT_HELLO
