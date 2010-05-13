
#if defined(HTTP_SUPPORT_OPENSSL)

#include <http/server_ssl_connection.hpp>
#include <http/server.hpp>
#include <http/handler_request_error.hpp>

#include <boost/config.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#ifdef BOOST_MSVC
# pragma warning(push)
# pragma warning(disable: 4503) // decorated name length exceeded, name was truncated
#endif

using namespace std;
using namespace boost;

namespace http
{
namespace server 
{

	ssl_connection::ssl_connection(http::host& h, boost::asio::ssl::context& c) :
		connection_base(h),
		context_(c),
		socket_(h.io_service(), c)
	{
		parser_.set_connection(this);
		log() << "Ctor SSL Connection.\r\n";
	}
	
	void ssl_connection::start()
	{
		socket_.async_handshake(asio::ssl::stream_base::server,
			boost::bind(&ssl_connection::handle_handshake, shared_from_this(),
				asio::placeholders::error));
	}
	
	void ssl_connection::handle_handshake(const asio::error_code& e)
	{
		if (!e)
		{
			log() << "SSL Handshake Sucessful\r\n";	
			parser_.initiate();
			
			socket_.async_read_some(boost::asio::buffer(buffer_),
				boost::bind(&ssl_connection::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
		else if (e != boost::asio::error::operation_aborted)
		{
			connection_manager_.stop(shared_from_this());
		}
	}

	void ssl_connection::handle_read(const boost::asio::error_code& e, std::size_t bytes_transferred)
	{
		if (!e)
		{			
			try
			{			
			unsigned result = parser_.parse(buffer_.data(), buffer_.data() + bytes_transferred);
			
			if (!(result & ev_parse::close_connection))
			{				
				socket_.async_read_some(boost::asio::buffer(buffer_),
						boost::bind(&ssl_connection::handle_read, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
			}
			else
			{
				log() << "Read SSL Connection Close\r\n";			
			}				
			}
			catch (const http::error& ex)
			{
			log() << format("Caught Request Excpt.: %1%.\r\n") % ex.what();

			request::handler_ptr error(new request::handler_error(response::structure::bad_request));
			error->set_close_connection();
			
			handler_queue().add_handler(error);
			}
			catch (const std::exception& ex)
			{
			log() << format("Caught Excpt.: %1%.\r\n") % ex.what();
			
			request::handler_ptr error(new request::handler_error(response::structure::internal_server_error));
			error->set_close_connection();
			
			handler_queue().add_handler(error);
			}
			
			handle_handler();	// Note outside of try {} block so that exceptions 
								// propagate to react function
		}
		else if (e != boost::asio::error::operation_aborted)
		{
			connection_manager_.stop(shared_from_this());
		}
	}
	
	void ssl_connection::handle_handler()
	{	
		if (handler_queue().can_start_handling_then_do())
		{
			log() << "SSL Handling\r\n";
			
			request::handler_ptr h = handler_queue().get_current_handler();
			
			pair<bool, vector<boost::asio::const_buffer> > res = h->react();
		
			log() << format("About to SSL send, %1%\r\n") % res.second.size();
			
			boost::asio::async_write(socket_, res.second,
				boost::bind(&ssl_connection::handle_write, shared_from_this(),
				boost::asio::placeholders::error, res.first));
		}
	}
	
	void ssl_connection::handle_write(const boost::asio::error_code& e, bool finished)
	{
		if (!e)
		{	
			request::handler_ptr h = handler_queue().get_current_handler();
			
			log() << format("SSL Sent, %1%\r\n") % (h->keep_alive() ? "keep-alive" : "close");
			
			if (finished)
			{			
				handler_queue().handled();
				
				if (h->keep_alive())
					handle_handler();
				else
					connection_manager_.stop(shared_from_this());
			}
			else
			{	
				pair<bool, vector<boost::asio::const_buffer> > res = h->react();
				
				log() << format("About to SSL send, %1%\r\n") % res.second.size();
				
				boost::asio::async_write(socket_, res.second,
					boost::bind(&ssl_connection::handle_write, shared_from_this(),
					boost::asio::placeholders::error, res.first));
			}
		}		
		else if (e != boost::asio::error::operation_aborted)
		{
			connection_manager_.stop(shared_from_this());
		}
	}
  
	void ssl_connection::close_down()
	{
		connection_manager_.stop(shared_from_this());
	}
	
	void ssl_connection::stop()
	{
		socket_.lowest_layer().close();
	}
	
	ssl_socket::lowest_layer_type& ssl_connection::socket()
	{
		return socket_.lowest_layer();
	}

} // namespace server
} // namespace http

#ifdef BOOST_MSVC
# pragma warning(default: 4503) // decorated name length exceeded, name was truncated
# pragma warning(pop)
#endif

#endif
