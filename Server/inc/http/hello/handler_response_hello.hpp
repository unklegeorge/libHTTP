
#ifndef HTTP_HANDLER_RESPONSE_HELLO
#define HTTP_HANDLER_RESPONSE_HELLO

#include <string>

#include <http/handler_response_base.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace response
{

class handler_hello :
	public handler_base
{
public:	
	handler_hello(response::structure_ptr r) :
		handler_base(r)
	{
		log() << "Ctor Hello handler\r\n";
	}
	
	virtual ~handler_hello()
	{
		log() << "Dtor Hello handler\r\n";
	}
	
	virtual bool parse(const char* begin, const char* end)
	{
		std::string str(begin, end);
		reply_ += str;
			
		return true;
	}
	
	virtual boost::tuple<bool, boost::any> react()
	{		
		return boost::make_tuple(true, reply_);
	}
	
private:	
	std::string reply_;
};

class handler_hello_ctor :
	public handler_ctor
{
public:
	virtual handler_ptr create(response::structure_ptr r)
	{
		return handler_ptr(new response::handler_hello(r));
	}
};

} // namespace response
} // namespace http

#endif // HTTP_HANDLER_RESPONSE_HELLO
