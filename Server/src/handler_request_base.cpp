
#include <http/handler_request_base.hpp>

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace http
{
namespace request 
{

void handler_base::append_to_buffers(std::vector<boost::asio::const_buffer> v)
{
	vec_.insert(vec_.end(), v.begin(), v.end());
}

void handler_base::append_to_buffers(boost::asio::const_buffer b)
{
	vec_.push_back(b);
}

void handler_base::create_response(unsigned status, string content_type, size_t content_length)
{	
	res_.reset(new response::structure(lexical_cast<string>(status), response::reply::status_strings::get_status(status), "1.1"));
			
	if (keep_alive_)
		res_->add_header("Connection","keep-alive");
	else
		res_->add_header("Connection","close");

	res_->add_header("Content-Type", content_type);
	res_->add_header("Content-Length", lexical_cast<string>(content_length));
}

void handler_base::create_response(unsigned status)
{	
	res_.reset(new response::structure(lexical_cast<string>(status), response::reply::status_strings::get_status(status), "1.1"));

	if (keep_alive_)
		res_->add_header("Connection","keep-alive");
	else
		res_->add_header("Connection","close");	
}

} // namespace request
} // namespace http
