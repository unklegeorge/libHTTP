
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <http/server.hpp>
#include <http/server_connection_base.hpp>
#include <http/handler_request_base.hpp>
#include <http/handler_request_error.hpp>
#include <http/debug_logger.hpp>

namespace fs = boost::filesystem;

namespace http
{
	host::host() : 
		io_service_(),
		acceptor_(io_service_)
# 		if defined(HTTP_SUPPORT_OPENSSL)
		,ssl_acceptor_(io_service_)
#		endif
	{}
	
	void host::bind_to(short port)
	{
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
		acceptor_.open(boost::asio::ip::tcp::v4());
		acceptor_.bind(endpoint);
		acceptor_.listen();
		
		server::connection_ptr new_connection(new server::connection(*this));		
		acceptor_.async_accept(new_connection->socket(),
			boost::bind(&host::handle_accept, this, new_connection,
			boost::asio::placeholders::error));	
	}	
	
	void host::unbind()
	{
		acceptor_.close();
	}

# 	if defined(HTTP_SUPPORT_OPENSSL)
	boost::asio::ssl::context& host::prepare_ssl_context(boost::asio::ssl::context_base::method m)
	{
		context_.reset(new boost::asio::ssl::context(io_service_, m));
		
		return *context_;
	}
	
	void host::ssl_bind_to(short port)
	{
		boost::asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
		ssl_acceptor_.open(asio::ip::tcp::v4());
		ssl_acceptor_.bind(endpoint);
		ssl_acceptor_.listen();
	
		server::ssl_connection_ptr new_connection(new server::ssl_connection(*this, *context_));	
		ssl_acceptor_.async_accept(new_connection->socket(),
			boost::bind(&host::ssl_handle_accept, this, new_connection,
			boost::asio::placeholders::error));		
	}
	
	void host::ssl_unbind()
	{
		ssl_acceptor_.close();
	}
#	endif

	void host::run()
	{
		io_service_.run();
	}	
	
	void host::stop()
	{
		io_service_.post(boost::bind(&host::handle_stop, this));
	}
	
	void host::handle_accept(server::connection_ptr new_connection, const boost::system::error_code& e)
	{
		if (!e)
		{
			log() << "Recieved connection.\r\n";
			connection_manager_.start(new_connection);
			
			new_connection.reset(new server::connection(*this));
			acceptor_.async_accept(new_connection->socket(),
				boost::bind(&host::handle_accept, this, new_connection,
				boost::asio::placeholders::error));
		}
		else if (e == boost::asio::error::connection_aborted)
		{
			acceptor_.async_accept(new_connection->socket(),
				boost::bind(&host::handle_accept, this, new_connection,
				boost::asio::placeholders::error));
		}	
	}
	
# 	if defined(HTTP_SUPPORT_OPENSSL)		
	void host::ssl_handle_accept(server::ssl_connection_ptr new_connection, const boost::asio::error_code& e)
	{
		if (!e)
		{
			log() << "Recieved SSL connection.\r\n";
			connection_manager_.start(new_connection);
			
			new_connection.reset(new server::ssl_connection(*this, *context_));
			ssl_acceptor_.async_accept(new_connection->socket(),
				boost::bind(&host::ssl_handle_accept, this, new_connection,
				boost::asio::placeholders::error));
		}
		else if (e == boost::asio::error::connection_aborted)
		{
			ssl_acceptor_.async_accept(new_connection->socket(),
				boost::bind(&host::ssl_handle_accept, this, new_connection,
				boost::asio::placeholders::error));
		}	
	}
#	endif

	void host::handle_stop()
	{
		acceptor_.close();
# 		if defined(HTTP_SUPPORT_OPENSSL)
		ssl_acceptor_.close();
#		endif
		connection_manager_.stop_all();
	}
	
	void host::add_handler(fs::path uri, request::handler_ctor_ptr handler)
	{
		handlers_.insert(std::make_pair(uri, handler));
	}
	
	request::handler_ptr host::get_handler(const fs::path uri, request::structure_ptr r) const
	{
		fs::path base_uri = uri;
		fs::path relative_uri;
		
		while (!base_uri.empty())
		{
		//	log() << boost::format("Checking %1%\r\n") % base_uri.native_file_string(); 
			
			request::handler_map_cit result = handlers_.find(base_uri);
			
			if (result != handlers_.end())
			{
				return result->second->create(r, base_uri, relative_uri);
			}
			
			relative_uri = base_uri.leaf() / relative_uri;
			base_uri = base_uri.branch_path();
		}
		return request::handler_ptr(new request::handler_error(response::structure::not_found)) ;	
	}

namespace server
{

} // namespace server

} // namespace http
