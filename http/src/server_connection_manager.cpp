
#include <http/server_connection_manager.hpp>

#include <algorithm>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include <http/debug_logger.hpp>

namespace http
{
namespace server
{

void connection_manager::start(connection_base_ptr c)
{
	log() << "Starting connection.\r\n";
	connections_.insert(c);
	c->start();
}

void connection_manager::stop(connection_base_ptr c)
{
	log() << "Stopping connection.\r\n";
	c->stop();
	log() << "Erasing connection.\r\n";
	connections_.erase(c);
}

void connection_manager::stop_all()
{
	log() << boost::format("Stopping %1% connections.\r\n") % connections_.size();
	
	while (connections_.begin() != connections_.end())
	{
		stop(*connections_.begin());
	}
	//std::for_each(connections_.begin(), connections_.end(),
	//	boost::bind(&connection_manager::stop, this, _1));
	//connections_.clear();
}

} // namespace server
} // namespace http
