
#include <http/client_connection_base.hpp>
#include <http/client_verb.hpp>

namespace http
{
namespace client 
{
namespace sync
{

connection_base::connection_base(std::string addr, unsigned short port) :
	io_service_(),
	resolver_(io_service_),
	query_(addr, boost::lexical_cast<std::string>(port))
{
	log() << "Ctor Base Connection.\r\n";
}

void connection_base::set_verb(const http::client::sync::verb* v) 
{ 
	boost::mutex::scoped_lock l(mutex_); // Maybe this ain't necessary ???
	
	verb_ = v; 
}

const http::client::sync::verb& connection_base::verb() const
{ 
	return *verb_; 
}
	

} // namespace sync
} // namespace client
} // namespace http
