
#include <http/server_connection.hpp>
#include <http/server.hpp>
#include <http/handler_request_error.hpp>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/format.hpp>

using namespace std;
using namespace boost;

namespace http
{
namespace server
{	
	connection::connection(http::host& h) :
		connection_base(h),
		socket_(h.io_service())
	{	
		parser_.set_connection(this);
		http::log() << "Ctor Connection.\r\n";
	}
	
	void connection::start()
	{
		parser_.initiate();
		
		socket_.async_read_some(boost::asio::buffer(buffer_),
			boost::bind(&connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void connection::handle_read(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (!e)
		{			
			try
			{			
			unsigned result = parser_.parse(buffer_.data(), buffer_.data() + bytes_transferred);
			
			if (!(result & ev_parse::close_connection))
			{				
				socket_.async_read_some(boost::asio::buffer(buffer_),
						boost::bind(&connection::handle_read, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred)
						);
			}
			else
			{
				log() << "Read Connection Close\r\n";			
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
	
	void connection::handle_handler()
	{	
		if (handler_queue().can_start_handling_then_do())
		{
			log() << "Handling\r\n";
			
			request::handler_ptr h = handler_queue().get_current_handler();			
			pair<bool, vector<boost::asio::const_buffer> > res = h->react();
		
			log() << format("About to send, %1%\r\n") % res.second.size();
			
			boost::asio::async_write(socket_, res.second,
				boost::bind(&connection::handle_write, shared_from_this(),
				boost::asio::placeholders::error, res.first));
		}
	}
	
	void connection::handle_write(const boost::system::error_code& e, bool finished)
	{
		if (!e)
		{	
			request::handler_ptr h = handler_queue().get_current_handler();
			
			log() << format("Sent, %1%\r\n") % (h->keep_alive() ? "keep-alive" : "close");
			
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
				
				log() << format("About to send, %1%\r\n") % res.second.size();
				
				boost::asio::async_write(socket_, res.second,
					boost::bind(&connection::handle_write, shared_from_this(),
					boost::asio::placeholders::error, res.first));
			}
		}		
		else if (e != boost::asio::error::operation_aborted)
		{
			connection_manager_.stop(shared_from_this());
		}
	}
  
	void connection::close_down()
	{
		connection_manager_.stop(shared_from_this());
	}
	
	void connection::stop()
	{
		socket_.close();
	}
	
	boost::asio::ip::tcp::socket& connection::socket()
	{
		return socket_;
	}

} // namespace server
} // namespace http
