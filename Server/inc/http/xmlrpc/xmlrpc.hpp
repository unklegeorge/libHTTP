#ifndef HTTP_XMLRPC
#define HTTP_XMLRPC

#include <sstream>
#include <stdexcept>
#include <vector>
#include <map>
#include <iterator>
#include <asio/buffer.hpp>
namespace boost__asio = asio;
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/function.hpp>
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>

#include <tinyxml/tinyxml.h>
#include <b64/encode.h>

#include <http/xmlrpc/errors.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace xmlrpc
{

class error : public std::exception 
{
public:
	error(const unsigned code, const std::string& message) :
		code_(code),
		str_(message)
	{}
	virtual ~error() throw () {}
	
	virtual const char *what() const throw () 
	{
		return (str_.c_str());
	}	
	
	unsigned code() const { return code_; }
	std::string str() const { return str_; }
	
private:
	unsigned code_;
	std::string str_;
};

struct param_vec;
struct param_map;

typedef std::vector<char> base64_data;

typedef boost::variant<
	int,
	double,
	std::string,
	boost::posix_time::ptime,
	base64_data,
	boost::recursive_wrapper<param_vec>,
	boost::recursive_wrapper<param_map>
    > param;
	
struct param_vec 
{ 
public:
	param_vec() {}
	param_vec(std::vector<param> vec) : v(vec) {}
	std::vector<param>& operator()() { return v; }
	const std::vector<param>& operator()() const { return v; }
	
private:
	std::vector<param> v; 
};

struct param_map 
{ 
public:
	param_map() {}
	param_map(std::map<std::string, param> map) : m(map) {}
	std::map<std::string, param>& operator()() { return m; }
	const std::map<std::string, param>& operator()() const { return m; }
	
private:
	std::map<std::string, param> m; 
};

typedef std::vector<param> param_arguments;
typedef boost::function<param (param_arguments)> procedure;

class procedure_manager
{
public:
	void add_procedure(std::string method_name, procedure proc)
	{
		proc_map_.insert(std::make_pair(method_name, proc));
	}
	
	procedure get_procedure(std::string method_name)
	{
		std::map<std::string, procedure>::iterator result = proc_map_.find(method_name);
		
		if (result != proc_map_.end())
		{
			return (*result).second;
		}
		else
		{
			return 0;
		}
	}

private:
	std::map<std::string, procedure> proc_map_;
};

class param_to_xml : 
	public boost::static_visitor<TiXmlElement*>
{
public:
    TiXmlElement* operator()(const int& i) const
    {
        TiXmlElement* elem = new TiXmlElement("i4");
		TiXmlText* value = new TiXmlText(boost::lexical_cast<std::string>(i).c_str());
		
		elem->LinkEndChild(value);
		return elem;
    }
    TiXmlElement* operator()(const double& d) const
    {
        TiXmlElement* elem = new TiXmlElement("double");
		TiXmlText* value = new TiXmlText(boost::lexical_cast<std::string>(d).c_str());
		
		elem->LinkEndChild(value);
		return elem;
    }
    TiXmlElement* operator()(const std::string& str) const
    {
        TiXmlElement* elem = new TiXmlElement("string");
		TiXmlText* value = new TiXmlText(str.c_str());
		
		elem->LinkEndChild(value);
		return elem;
    }
    TiXmlElement* operator()(const std::vector<char>& vec) const
    {
        TiXmlElement* elem = new TiXmlElement("base64");
		
		base64::encoder enc;
		std::ostringstream osstr;
		
		enc.encode(vec, osstr);
		TiXmlText* value = new TiXmlText(osstr.str().c_str());
		
		elem->LinkEndChild(value);
		return elem;
    }
    TiXmlElement* operator()(const boost::posix_time::ptime& tim) const
    {
        TiXmlElement* elem = new TiXmlElement("dateTime.iso8601");
		
		std::stringstream sstr;
		boost::posix_time::time_facet* facet = new boost::posix_time::time_facet("%Y%m%dT%H:%M:%S");
		sstr.imbue(std::locale(std::cout.getloc(), facet));
		sstr << tim;
		
		TiXmlText* value = new TiXmlText(sstr.str().c_str());
		
		elem->LinkEndChild(value);
		return elem;
    }
	TiXmlElement* operator()(const param_vec& params) const
	{
        TiXmlElement* elem = new TiXmlElement("array");
		TiXmlElement* data = new TiXmlElement("data");
		
		elem->LinkEndChild(data);
		
		for(std::vector<param>::const_iterator it=params().begin(); it!=params().end(); ++it)
		{
		    TiXmlElement* value = new TiXmlElement("value");
			data->LinkEndChild(value);
			value->LinkEndChild(boost::apply_visitor(param_to_xml(), *it));
		}
		
		return elem;
	}
    TiXmlElement* operator()(const param_map& map) const
    {
        TiXmlElement* elem = new TiXmlElement("struct");
		
		for(std::map<std::string, param>::const_iterator it=map().begin(); it!=map().end(); ++it)
		{
			TiXmlElement* member = new TiXmlElement("member");
			elem->LinkEndChild(member);		
	
			TiXmlElement* member_name = new TiXmlElement("name");
			member->LinkEndChild(member_name);
			TiXmlText* member_text = new TiXmlText((*it).first.c_str());
			member_name->LinkEndChild(member_text);
		
		    TiXmlElement* value = new TiXmlElement("value");
			member->LinkEndChild(value);
			value->LinkEndChild(boost::apply_visitor(param_to_xml(), (*it).second));
		}
		
		return elem;
    }
};		

param xml_to_param(TiXmlNode* node);

} // namespace xmlrpc
} // namespace http

#endif // HTTP_XMLRPC
