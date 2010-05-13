
#ifndef HTTP_CLIENT_SSL_CONNECTION
#define HTTP_CLIENT_SSL_CONNECTION

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

typedef asio::ssl::stream<asio::ip::tcp::socket> ssl_socket;

class ssl_connection :
	public boost::enable_shared_from_this<ssl_connection>,
	public connection_base
{
public:
	ssl_connection(std::string addr, unsigned short port, boost__asio::ssl::context_base::method m);
	
	~ssl_connection()
	{
		log() << "Dtor Client SSL Connection.\r\n";
	}
	
	void connect();	
	void disconnect();
	boost::tuple<bool, boost::any> send_all_react(std::vector<boost__asio::const_buffer>);
	
	ssl_socket& socket() { return socket_; }
	boost__asio::ssl::context& context() { return context_; }
	
private:
	boost__asio::ssl::context context_;
	ssl_socket socket_;
	bool connected_;
};

typedef boost::shared_ptr<ssl_connection> ssl_connection_ptr;

} // namespace sync
} // namespace client
} // namespace http

#endif // HTTP_CLIENT_SSL_CONNECTION
