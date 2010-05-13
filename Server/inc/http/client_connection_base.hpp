
#ifndef HTTP_CLIENT_CONNECTION_BASE
#define HTTP_CLIENT_CONNECTION_BASE

#include <deque>

#include <boost/asio.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/any.hpp>

#include <http/parser_response.hpp>
#include <http/handler_queue.hpp>

namespace http
{

namespace response 
{
class handler_base;
typedef boost::shared_ptr<handler_base> handler_ptr;
//typedef std::deque<handler_ptr> handler_queue;
}

namespace client 
{
namespace sync
{

class verb;

class connection_base :
	private boost::noncopyable
{
public:
	connection_base(std::string addr, unsigned short port);	
	virtual ~connection_base() {};
	
	virtual void connect() = 0;	
	virtual void disconnect() = 0;
	virtual boost::tuple<bool, boost::any> send_all_react(std::vector<boost::asio::const_buffer>) = 0;
	
	void set_verb(const http::client::sync::verb* v);
	const http::client::sync::verb& verb() const;
	
	handler_queue_manager<response::handler_ptr>& handler_queue() 
		{ return handler_queue_;}
	const handler_queue_manager<response::handler_ptr>& handler_queue() const 
		{ return handler_queue_;}
		
protected:
	boost::asio::io_service io_service_;
	boost::asio::ip::tcp::resolver resolver_;
	boost::asio::ip::tcp::resolver::query query_;
	boost::asio::ip::tcp::resolver::iterator endpoint_iterator_;
	
	boost::array<char, 8192> buffer_;
	response::parser parser_;
	const http::client::sync::verb* verb_;
	handler_queue_manager<response::handler_ptr> handler_queue_;
	
	mutable boost::mutex mutex_;
};

typedef boost::shared_ptr<connection_base> connection_base_ptr;

} // namespace sync
} // namespace client
} // namespace http

#endif // HTTTP_CLIENT_CONNECTION_BASE
