
#ifndef HTTP_CLIENT_VERB
#define HTTP_CLIENT_VERB

#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>

#include <http/client_connection_base.hpp>
#include <http/handler_ctor.hpp>
#include <http/structure_response.hpp>
#include <http/structure_request.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace client 
{
namespace sync
{

class verb
{
public:
	verb(connection_base_ptr con, const std::string& verb, 
		const boost::filesystem::path& uri, const std::string& ver,
		response::handler_ctor* h) :
			connection_(con),
			req_(new request::structure(verb, uri, ver)),
			handler_ctor_(h)
	{
		connection_->set_verb(this);
		connection_->connect();
		
		log() << "Ctor verb\r\n";
	}
	~verb()
	{
		log() << "Dtor verb\r\n";
	}
	
	response::handler_ctor_ptr get_handler() const { return handler_ctor_; }
	request::structure& req() { return *req_; }

	void inline reset_buffers() 
	{ 
		vec_.clear(); 
	}
	void append_to_buffers(std::vector<boost::asio::const_buffer> v)
	{ 
		vec_.insert(vec_.end(), v.begin(), v.end());
	}
	void append_to_buffers(boost::asio::const_buffer b)
	{
		vec_.push_back(b);	
	}
	std::vector<boost::asio::const_buffer>& vec() { return vec_; }
	connection_base_ptr connection() { return connection_; }
	
protected:
	connection_base_ptr connection_;
	request::structure_ptr req_;
	response::handler_ctor_ptr handler_ctor_;
	
private:	
	std::vector<boost::asio::const_buffer> vec_;
};

} // namespace sync
} // namespace client
} // namespace http

#endif // HTTP_CLIENT_VERB
