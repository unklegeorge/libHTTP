
#ifndef HTTP_HANDLER_RESPONSE_ERROR
#define HTTP_HANDLER_RESPONSE_ERROR

#include <string>
#include <boost/any.hpp>

#include <http/handler_response_base.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace response
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
	
	virtual boost::tuple<bool, boost::any> react()
	{		
		return boost::make_tuple(false, detail::void_class());
	}
	
private:
	unsigned status_code_;
	
};

} // namespace response
} // namespace http

#endif // HTTP_HANDLER_RESPONSE_ERROR
