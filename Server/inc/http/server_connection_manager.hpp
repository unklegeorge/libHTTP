
#ifndef HTTP_SERVER_CONNECTION_MANAGER_HPP
#define HTTP_SERVER_CONNECTION_MANAGER_HPP

#include <set>
#include <boost/noncopyable.hpp>

#include <http/server_connection_base.hpp>

namespace http 
{
namespace server
{

class connection_manager : 
	private boost::noncopyable
{
public:
	void start(connection_base_ptr c);

	void stop(connection_base_ptr c);

	void stop_all();

private:
	std::set<connection_base_ptr> connections_;
};

} // namespace server
} // namespace http

#endif // HTTP_SERVER_CONNECTION_MANAGER_HPP
