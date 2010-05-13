
#ifndef HTTP_SERVER_HELLO
#define HTTP_SERVER_HELLO

#include <http/server.hpp>

namespace http
{
namespace server 
{

void add_hello_handler(http::host& h, boost::filesystem::path uri);

void add_hello_handler(http::host_ptr h, boost::filesystem::path uri);

void add_hello_xml_handler(http::host_ptr h, boost::filesystem::path uri);

} // namespace server
} // namespace http

#endif // HTTP_SERVER_HELLO
