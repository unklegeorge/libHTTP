
#ifndef HTTP_HANDLER_REQUEST_BASE
#define HTTP_HANDLER_REQUEST_BASE

#include <map>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <http/handler_ctor.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace request 
{

class handler_base :
	private boost::noncopyable
{
public:		
	handler_base() :
		keep_alive_(true)
	{ 
		log() << "Ctor Handler\r\n"; 
	}
	handler_base(request::structure_ptr r,
		boost::filesystem::path base_uri, boost::filesystem::path relative_uri) :
			req_(r),
			base_uri_(base_uri),
			relative_uri_(relative_uri),
			keep_alive_(true)
	{ 
		log() << "Ctor Handler\r\n"; 
	}
	virtual ~handler_base() 
	{ 
		log() << "Dtor Handler\r\n"; 
	}
	
	virtual bool parse(const char* begin, const char* end) = 0;
	virtual std::pair<bool, std::vector<boost::asio::const_buffer> > react() = 0;
	
	void set_close_connection() { keep_alive_ = false; }
	bool keep_alive() { return keep_alive_; }
		
protected:
	const request::structure_ptr req_;
	
	void inline reset_buffers() { vec_.clear(); }
	void append_to_buffers(std::vector<boost::asio::const_buffer> v);
	void append_to_buffers(boost::asio::const_buffer b);
	std::vector<boost::asio::const_buffer>& vec() { return vec_; }
	
	void create_response(unsigned status, std::string content_type, size_t content_length);
	void create_response(unsigned status);
	
	response::structure& res() { return *res_; }
	
private:
	response::structure_ptr res_;
	boost::filesystem::path base_uri_;
	boost::filesystem::path relative_uri_;
	
	std::vector<boost::asio::const_buffer> vec_;
	bool keep_alive_;
};

typedef boost::shared_ptr<handler_base> handler_ptr;

} // namespace request
} // namespace http

#endif // HTTP_HANDLER_REQUEST_BASE
