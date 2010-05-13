
#ifndef HTTP_HANDLER_CTOR
#define HTTP_HANDLER_CTOR

#include <http/structure_response.hpp>
#include <http/structure_request.hpp>
#include <http/debug_logger.hpp>

namespace http
{

class handler_ctor :
	private boost::noncopyable
{};

namespace request
{

class handler_base;
typedef boost::shared_ptr<handler_base> handler_ptr;

class handler_ctor :
	public http::handler_ctor
{
public:
	handler_ctor()
	{
		log() << "Ctor Req. Handler\r\n";
	}	
	virtual ~handler_ctor()
	{
		log() << "Dtor Req. Handler\r\n";
	}
	virtual handler_ptr create(request::structure_ptr r, 
		boost::filesystem::path base_uri, boost::filesystem::path relative_uri) = 0;
};

typedef boost::shared_ptr<request::handler_ctor> handler_ctor_ptr;
typedef std::map<boost::filesystem::path, handler_ctor_ptr> handler_map;
typedef handler_map::const_iterator handler_map_cit;

} // namespace request

namespace response
{

class handler_base;
typedef boost::shared_ptr<handler_base> handler_ptr;

class handler_ctor :
	public http::handler_ctor
{
public:
	handler_ctor()
	{
		log() << "Ctor Res. Handler\r\n";
	}	
	virtual ~handler_ctor()
	{
		log() << "Dtor Res. Handler\r\n";
	}
	virtual handler_ptr create(response::structure_ptr r) = 0;
};

typedef boost::shared_ptr<response::handler_ctor> handler_ctor_ptr;
typedef std::map<boost::filesystem::path, handler_ctor_ptr> handler_map;
typedef handler_map::const_iterator handler_map_cit;

} // namespace response

} // namespace http

#endif // HTTP_HANDLER_CTOR
