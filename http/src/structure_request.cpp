
#include <http/structure_request.hpp>

namespace http
{

namespace request
{

structure::~structure()
{}

void structure::to_buffers(std::vector<boost::asio::const_buffer>& v)
{
	v.push_back(boost::asio::buffer(verb_));
	v.push_back(boost::asio::buffer(misc_strings::space));
	v.push_back(boost::asio::buffer(uri_.string()));
	v.push_back(boost::asio::buffer(misc_strings::space));
	v.push_back(boost::asio::buffer(misc_strings::httpslash));
	v.push_back(boost::asio::buffer(version()));
	v.push_back(boost::asio::buffer(misc_strings::crlf));
	
	structure_base::to_buffers(v);
}

} // namespace request
} // namespace http
