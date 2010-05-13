
#ifndef HTTP_HANDLER_RESPONSE_BASE
#define HTTP_HANDLER_RESPONSE_BASE

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
namespace response 
{

namespace detail
{
class void_class
{};
}

class handler_base :
	private boost::noncopyable
{
public:	
	handler_base(response::structure_ptr r) :
		res_(r),
		keep_alive_(true)
	{ 
		log() << "Ctor Handler\r\n"; 
	}	
	handler_base() :
		keep_alive_(true)
	{ 
		log() << "Ctor Handler\r\n"; 
	}
	virtual ~handler_base() 
	{ 
		log() << "Dtor Handler\r\n"; 
	}
	
	virtual bool parse(const char* begin, const char* end) = 0;
	virtual boost::tuple<bool, boost::any> react() = 0;
	
	void set_close_connection() { keep_alive_ = false; }
	bool keep_alive() { return keep_alive_; }
	
protected:
	const response::structure_ptr res_;
	
private:
	bool keep_alive_;
};

typedef boost::shared_ptr<handler_base> handler_ptr;

} // namespace response
} // namespace http

#endif // HTTP_HANDLER_RESPONSE_BASE
