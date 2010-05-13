
#include <http/structure_response.hpp>

namespace http
{
namespace response
{

void structure::to_buffers(std::vector<boost::asio::const_buffer>& v)
{
	v.push_back(boost::asio::buffer(version_));
	v.push_back(boost::asio::buffer(misc_strings::space));
	v.push_back(boost::asio::buffer(status_code_));
	v.push_back(boost::asio::buffer(misc_strings::space));
	v.push_back(boost::asio::buffer(reason_));
	
	structure_base::to_buffers(v);
}

namespace reply
{

namespace status_strings {

std::string get_status(unsigned status)
{
	switch (status)
	{
	case structure::ok:
		return ok;
	case structure::created:
		return created;
	case structure::accepted:
		return accepted;
	case structure::no_content:
		return no_content;
	case structure::multiple_choices:
		return multiple_choices;
	case structure::moved_permanently:
		return moved_permanently;
	case structure::moved_temporarily:
		return moved_temporarily;
	case structure::not_modified:
		return not_modified;
	case structure::bad_request:
		return bad_request;
	case structure::unauthorized:
		return unauthorized;
	case structure::forbidden:
		return forbidden;
	case structure::not_found:
		return not_found;
	case structure::internal_server_error:
		return internal_server_error;
	case structure::not_implemented:
		return not_implemented;
	case structure::bad_gateway:
		return bad_gateway;
	case structure::service_unavailable:
		return service_unavailable;
	default:
		return internal_server_error;
	}
}

} // namespace status_strings
} // namespace reply

} // namespace response
} // namespace http
