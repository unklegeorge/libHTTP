
#ifndef HTTP_SERVER_CONNECTION_BASE
#define HTTP_SERVER_CONNECTION_BASE

#include <deque>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <http/parser_request.hpp>
#include <http/handler_queue.hpp>

namespace http
{

class host;

namespace request 
{

class handler_base;
typedef boost::shared_ptr<handler_base> handler_ptr;
//typedef std::deque<request::handler_ptr> handler_queue;

};

namespace server
{

class connection_manager;
class connection_base;


class connection_base :
	private boost::noncopyable 
{
public:
	connection_base(http::host& s);	
	virtual ~connection_base() {};
	
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void close_down() = 0;	
	
	const http::host& host() const { return host_; }
	
	handler_queue_manager<request::handler_ptr>& handler_queue() 
		{ return handler_queue_;}
	const handler_queue_manager<request::handler_ptr>& handler_queue() const 
		{ return handler_queue_;}

protected:
	virtual void handle_write(const boost::system::error_code& error, bool finished) = 0;
	virtual void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred) = 0;
	virtual void handle_handler() = 0;
	
	boost::array<char, 8192> buffer_;
	
	const http::host& host_;
	connection_manager& connection_manager_;
	request::parser parser_;
	
	handler_queue_manager<request::handler_ptr> handler_queue_;
};

typedef boost::shared_ptr<connection_base> connection_base_ptr;

} // namespace server
} // namespace http

#endif // HTTP_SERVER_CONNECTION_BASE
