
#include <boost/bind.hpp>

#include <http/client_connection.hpp>
#include <http/handler_response_base.hpp>

namespace http
{
namespace client 
{
namespace sync
{

connection::connection(std::string addr, unsigned short port) :
	connection_base(addr, port),
	socket_(io_service_),
	connected_(false)
{
	parser_.set_connection(this);
	
	log() << "Ctor Client Connection.\r\n";
}

void connection::connect()
{
	boost::mutex::scoped_lock l(mutex_);
	
	if (!connected_)
	{	
		endpoint_iterator_ = resolver_.resolve(query_);	
		boost::asio::ip::tcp::resolver::iterator end;
		boost::system::error_code error = boost::asio::error::host_not_found;
		
		while (error && endpoint_iterator_ != end)
		{
			socket_.close();
			socket_.connect(*endpoint_iterator_++, error);
		}
		if (error)
			throw boost::system::system_error(error);
		
		connected_ = true;
	}
}

void connection::disconnect()
{
	boost::mutex::scoped_lock l(mutex_);
	
	if (connected_)
	{
		socket_.close();
		connected_ = false;
	}
}
	
boost::tuple<bool, boost::any> connection::send_all_react(std::vector<boost::asio::const_buffer> vec)
{
	try
	{
	boost::system::error_code error;
	
	boost::asio::write(socket_, vec, boost::asio::transfer_all(), error);
	
	parser_.initiate();

	for (;;)
	{
		size_t len = socket_.read_some(boost::asio::buffer(buffer_), error);
			
		if (error == boost::asio::error::eof)
			break; 			// Connection closed cleanly by peer.
		else if (error)
			throw boost::system::system_error(error); 	// Some other error.
			
	  	unsigned result = parser_.parse(buffer_.data(), buffer_.data() + len);
	
		if ((result & ev_parse::close_connection))
		{
			log() << "Client Connection Close\r\n";
			
			disconnect();
			break;
		}	
		if (result & ev_parse::completed_transaction)
		{
			log() << "Completed Transaction\r\n";	
			break;
		}	
	}
	
	parser_.terminate();
	
	}	
	catch (const boost::system::error_code& ex)
	{
	log() << boost::format("Asio Error: %1%.\r\n") % ex;	
	throw error((boost::format("Asio: %1%") % ex).str());
	}
	catch (const http::error& ex)
	{
	log() << boost::format("Caught Request Excpt.: %1%.\r\n") % ex.what();
	throw error((boost::format("Request: %1%") % ex.what()).str());
	}
	catch (const std::exception& ex)
	{
	log() << boost::format("Caught Excpt.: %1%.\r\n") % ex.what();
	throw ex;
	}
	
	if (handler_queue().can_start_handling_then_do())
	{
		response::handler_ptr h = handler_queue().get_current_handler();
		
		boost::tuple<bool, boost::any> result = h->react();
		
		handler_queue().handled();
		
		return result;
	}
	
	throw std::logic_error("Unexpected program behavior in client connection");
}

} // namespace sync
} // namespace client
} // namespace http
