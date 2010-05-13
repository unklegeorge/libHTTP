
#ifndef HTTP_SERVER_CONNECTION
#define HTTP_SERVER_CONNECTION

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

class connection :
	public boost::enable_shared_from_this<connection>,
	public connection_base
{
public:
	connection(http::host& s);
	
	virtual ~connection()
	{
		http::log() << "Dtor Connection.\r\n";
	}
	
	boost::asio::ip::tcp::socket& socket();

	virtual void close_down();
	virtual void start();
	virtual void stop();	
	
protected:
	void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error, bool finished);
	void handle_handler();

	boost::asio::ip::tcp::socket socket_;
};

typedef boost::shared_ptr<connection> connection_ptr;

} // namespace server
} // namespace http

#endif // HTTP_SERVER_CONNECTION
