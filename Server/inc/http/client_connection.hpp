
#ifndef HTTP_CLIENT_CONNECTION
#define HTTP_CLIENT_CONNECTION

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>
#include <boost/format.hpp>

#include <http/client_connection_base.hpp>
#include <http/parser_base.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace client 
{
namespace sync
{

class connection :
	public boost::enable_shared_from_this<connection>,
	public connection_base
{
public:
	connection(std::string addr, unsigned short port);
	
	~connection()
	{
		log() << "Dtor Client Connection.\r\n";
	}
	
	void connect();	
	void disconnect();
	boost::tuple<bool, boost::any> send_all_react(std::vector<boost::asio::const_buffer>);
	boost::asio::ip::tcp::socket& socket() { return socket_; }
	
private:
	boost::asio::ip::tcp::socket socket_;
	bool connected_;
};

typedef boost::shared_ptr<connection> connection_ptr;

} // namespace sync
} // namespace client
} // namespace http

#endif // HTTP_CLIENT_CONNECTION
