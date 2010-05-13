
#ifndef HTTP_SERVER
#define HTTP_SERVER

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <http/handler_ctor.hpp>
#include <http/server_connection.hpp>
#if defined(HTTP_SUPPORT_OPENSSL)
#	include <http/server_ssl_connection.hpp>
#endif
#include <http/server_connection_manager.hpp>
#include <http/debug_logger.hpp>

namespace http
{

class host :
	boost::noncopyable
{
public:
	host();
	
	void bind_to(short port);
	void unbind();
# 	if defined(HTTP_SUPPORT_OPENSSL)
	boost::asio::ssl::context& prepare_ssl_context(boost::asio::ssl::context_base::method m);
	void ssl_bind_to(short port);
	void ssl_unbind();
#	endif
	
	void run();
	void stop();
	
	void handle_accept(server::connection_ptr new_connection, const boost::system::error_code& e);
# 	if defined(HTTP_SUPPORT_OPENSSL)
	void ssl_handle_accept(server::ssl_connection_ptr new_connection, const boost::system::error_code& error);
#	endif
	void handle_stop();
	
	void add_handler(boost::filesystem::path uri, request::handler_ctor_ptr handler);
	request::handler_ptr get_handler(const boost::filesystem::path uri, request::structure_ptr r) const;
	
	boost::asio::io_service& io_service() { return io_service_; }
	server::connection_manager& manager() { return connection_manager_; }

protected:
	boost::asio::io_service io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;
# 	if defined(HTTP_SUPPORT_OPENSSL)
	boost::asio::ip::tcp::acceptor ssl_acceptor_;
	boost::scoped_ptr<boost::asio::ssl::context> context_;
#	endif
	server::connection_manager connection_manager_;
	
	request::handler_map handlers_;
};

typedef boost::shared_ptr<http::host> host_ptr;

namespace server
{

} // namespace server

} // namespace http

#endif // HTTP_SERVER
