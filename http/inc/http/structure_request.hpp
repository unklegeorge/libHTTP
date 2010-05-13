
#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <http/structure_base.hpp>

namespace http
{
namespace request
{

class structure :
	public structure_base
{
public:	
	structure(const std::string& verb, const boost::filesystem::path& u, const std::string& ver) :
		verb_(verb),
		uri_(u),
		version_(ver)
	{}
	virtual ~structure();
	
	virtual void to_buffers(std::vector<boost::asio::const_buffer>& v);
	
    const std::string& verb() { return verb_; }	
	const boost::filesystem::path& uri() { return uri_; }
	const std::string& version() { return version_; }
	
protected:	
	// These are immutable after construction. the inherited headers are not.
	std::string verb_;	
	boost::filesystem::path uri_;
	std::string version_;
};

typedef boost::shared_ptr<structure> structure_ptr;

} // namespace request

} // namespace http

#endif // HTTP_REQUEST
