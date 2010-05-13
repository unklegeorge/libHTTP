
#ifndef HTTP_HANDLER_REQUEST_HELLO
#define HTTP_HANDLER_REQUEST_HELLO

#include <string>
#include <boost/lexical_cast.hpp>

#include <http/handler_request_base.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace request
{

typedef std::map<std::string, std::string> string_map;

class handler_hello :
	public handler_base
{
public:	
	handler_hello(request::structure_ptr r,
		boost::filesystem::path base_uri, boost::filesystem::path relative_uri) :
			handler_base(r, base_uri, relative_uri)
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
		name_ += str;
		
		return true;
	}
	
	virtual std::pair<bool, std::vector<boost::asio::const_buffer> > react()
	{
		log() << "Responding\r\n";
		name_ = "Hello " + name_;
		
		create_response(response::structure::ok);
		
		res().add_header("Content-Type", "text");
		res().add_header("Content-Length", boost::lexical_cast<std::string>(name_.length()));
		
		reset_buffers();
		res().to_buffers(vec());
		
		append_to_buffers(boost::asio::const_buffer(name_.c_str(), name_.length()));
				
		return make_pair(true, vec());
	}
	
private:
	std::string name_;
	
};

class handler_hello_ctor :
	public handler_ctor
{
public:
	virtual handler_ptr create(request::structure_ptr r,
		boost::filesystem::path base_uri, boost::filesystem::path relative_uri)
	{
		return handler_ptr(new request::handler_hello(r, base_uri, relative_uri));
	}
};

} // namespace request
} // namespace http

#endif // HTTP_HANDLER_REQUEST_HELLO
