
#ifndef HTTP_CLIENT_VERB_POST
#define HTTP_CLIENT_VERB_POST

#include <boost/asio.hpp>
#include <boost/format.hpp>

#include <http/client_verb.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace client 
{
namespace sync
{

class post :
	public verb
{
public:
	post(connection_base_ptr con, const boost::filesystem::path& uri, 
		const std::string& ver, response::handler_ctor* h) :
			verb(con, "POST", uri, ver, h)
	{
		log() << "Ctor POST\r\n";
	}
	~post()
	{
		log() << "Dtor POST\r\n";
	}
};

} // namespace sync
} // namespace client
} // namespace http

#endif // HTTP_CLIENT_VERB_POST
