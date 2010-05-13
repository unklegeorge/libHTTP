
#include <http/server_connection_base.hpp>
#include <http/server.hpp>

namespace http
{
namespace server
{
	connection_base::connection_base(http::host& s) :
		host_(s),
		connection_manager_(s.manager())
	{}

} // namespace server
} // namespace http
