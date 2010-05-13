
#ifndef HTTP_HTTP_OBJECT_BASE
#define HTTP_HTTP_OBJECT_BASE

#include <string>
#include <map>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <http/debug_logger.hpp>

namespace http
{
namespace misc_strings 
{

const char space[] = { ' ' };
const char name_value_separator[] = { ':', ' ' };
const char crlf[] = { '\r', '\n' };
const char httpslash[] = { 'H', 'T', 'T', 'P', '/' };

} // namespace misc_strings

typedef std::map<std::string, std::string> string_map;
typedef std::map<std::string, std::string>::iterator string_map_it;
typedef std::map<std::string, std::string>::const_iterator string_map_cit;

class structure_base :
	private boost::noncopyable
{
public:	
	structure_base()
	{
		log() << "Ctor Structre Base\r\n";
		newest_ = headers_.end();
	}
	virtual ~structure_base()
	{
		log() << "Dtor Structre Base\r\n";
	}

	bool add_header(const std::string& header, const std::string& value)
	{
		std::string header_name = header;
		boost::to_lower(header_name);
		
		std::pair<string_map_it, bool> retn = headers_.insert(
			make_pair(header_name, value));
			
		newest_ = retn.first;
		return retn.second;
	}
	
	bool append_header(const std::string& value)
	{
		if (newest_ == headers_.end())
			return false;
		
		newest_->second += " " + value;
		return true; 
	}
	
	boost::tuple<bool, std::string> get_value(const std::string& header)
	{
		string_map_it result = headers_.find(header);
		
		if (result == headers_.end())
			return boost::make_tuple(false, "");
		else
			return boost::make_tuple(true, result->second);
	}
	
	virtual void to_buffers(std::vector<boost::asio::const_buffer>& v)
	{
		for (string_map_it i=headers_.begin(); i!=headers_.end(); ++i)
		{
			v.push_back(boost::asio::buffer(i->first));
			v.push_back(boost::asio::buffer(misc_strings::name_value_separator));
			v.push_back(boost::asio::buffer(i->second));
			v.push_back(boost::asio::buffer(misc_strings::crlf));
		}
		
		v.push_back(boost::asio::buffer(misc_strings::crlf));
	};
	
protected:	
	string_map headers_;
	string_map_it newest_;
};

} // namespace http

#endif // HTTP_HTTP_OBJECT_BASE
