
#if defined(HTTP_SUPPORT_OPENSSL)

#include <boost/bind.hpp>

#include <http/client_ssl_connection.hpp>
#include <http/handler_response_base.hpp>

namespace http
{
namespace client 
{
namespace sync
{

ssl_connection::ssl_connection(std::string addr, unsigned short port, boost::asio::ssl::context_base::method m) :
	connection_base(addr, port),
	context_(io_service_, m),
	socket_(io_service_, context_),
	connected_(false)
{
	parser_.set_connection(this);
	
	log() << "Ctor Client SSL Connection.\r\n";
}

void ssl_connection::connect()
{
	boost::mutex::scoped_lock l(mutex_);
	
	if (!connected_)
	{		
		endpoint_iterator_ = resolver_.resolve(query_);	
		boost::asio::ip::tcp::resolver::iterator end;
		boost::asio::error_code error = boost::asio::error::host_not_found;
		
		while (error && endpoint_iterator_ != end)
		{
			socket_.lowest_layer().close();
			socket_.lowest_layer().connect(*endpoint_iterator_++, error);
			if (!error)
				socket_.handshake(asio::ssl::stream_base::client, error);
		}
		if (error)
			throw boost::asio::system_error(error);

		log() << "SSL Handshake successful.\r\n";		
		connected_ = true;
	}
}

void ssl_connection::disconnect()
{
	boost::mutex::scoped_lock l(mutex_);
	
	if (connected_)
	{
		socket_.lowest_layer().close();
		connected_ = false;
	}
}
	
boost::tuple<bool, boost::any> ssl_connection::send_all_react(std::vector<boost::asio::const_buffer> vec)
{
	try
	{
	boost::asio::error_code error;
	
	boost::asio::write(socket_, vec, boost::asio::transfer_all(), error);	
	
	parser_.initiate();

	for (;;)
	{
		size_t len = socket_.read_some(boost::asio::buffer(buffer_), error);
			
		if (error == boost::asio::error::eof)
			break; 			// Connection closed cleanly by peer.
		else if (error)
			throw boost::asio::system_error(error); 	// Some other error.
			
	  	unsigned result = parser_.parse(buffer_.data(), buffer_.data() + len);
	
		if ((result & ev_parse::close_connection))
		{
			log() << "Client SSL Connection Close\r\n";
			
			disconnect();
			break;
		}	
		if (result & ev_parse::completed_transaction)
		{
			log() << "Completed SSL Transaction\r\n";	
			break;
		}	
	}
	
	parser_.terminate();
	
	}	
	catch (const boost::asio::error_code& ex)
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
	
	throw std::logic_error("Unexpected program behaviour in client ssl connection");
}

} // namespace sync
} // namespace client
} // namespace http

#endif
