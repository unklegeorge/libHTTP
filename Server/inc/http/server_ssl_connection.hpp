
#ifndef HTTP_SERVER_SSL_CONNECTION
#define HTTP_SERVER_SSL_CONNECTION

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>

#include <http/server_connection_base.hpp>
#include <http/server_connection_manager.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace server
{

class http::host;
typedef asio::ssl::stream<asio::ip::tcp::socket> ssl_socket;

class ssl_connection :
	public boost::enable_shared_from_this<ssl_connection>,
	public connection_base
{
public:
	ssl_connection(http::host& h, boost__asio::ssl::context& c);
	
	virtual ~ssl_connection()
	{
		log() << "Dtor SSL_Connection.\r\n";
	}
	
	ssl_socket::lowest_layer_type& socket();
	
	virtual void close_down();
	virtual void start();
	virtual void stop();	
	
protected:
	void handle_handshake(const boost__asio::error_code& error);
	void handle_read(const boost__asio::error_code& e, std::size_t bytes_transferred);
	void handle_write(const boost__asio::error_code& error, bool finished);	
	void handle_handler();
	
	boost__asio::ssl::context& context_;
	ssl_socket socket_;
};

typedef boost::shared_ptr<ssl_connection> ssl_connection_ptr;

} // namespace server
} // namespace http

#endif // HTTP_SERVER_SSL_CONNECTION
