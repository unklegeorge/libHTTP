
#ifndef HTTP_HANDLER_REQUEST_ERROR
#define HTTP_HANDLER_REQUEST_ERROR

#include <string>

#include <http/handler_request_base.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace request
{

typedef std::map<std::string, std::string> string_map;

class handler_error :
	public handler_base
{
public:	
	handler_error(unsigned s) :
		status_code_(s)
	{
		log() << "Ctor Error handler\r\n";
	}
	
	virtual ~handler_error()
	{
		log() << "Dtor Error handler\r\n";
	}
	
	virtual bool parse(const char* begin, const char* end)
	{
		// Ignores any content.
		
		return true;
	}
	
	virtual std::pair<bool, std::vector<boost::asio::const_buffer> > react()
	{
		log() << "Responding with Error\r\n";
		
		create_response(status_code_);
		
		reset_buffers();
		res().to_buffers(vec());
		
		return std::make_pair(true, vec());
	}
	
private:
	unsigned status_code_;
	
};

class handler_error_ctor :
	public handler_ctor
{
public:
	handler_error_ctor(unsigned s) :
		status_code_(s)
	{}
	
	virtual handler_ptr create(request::structure_ptr r)
	{
		return handler_ptr(new request::handler_error(status_code_));
	}
	
private:
	unsigned status_code_;
};

} // namespace request
} // namespace http

#endif // HTTP_HANDLER_REQUEST_ERROR
