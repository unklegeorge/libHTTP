
#ifndef HTTP_HANDLER_RESPONSE_HELLO_XML
#define HTTP_HANDLER_RESPONSE_HELLO_XML

#include <string>

#include <tinyxml/tinyxml.h>

#include <http/handler_response_base.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace response
{

class handler_hello_xml :
	public handler_base
{
public:	
	handler_hello_xml(response::structure_ptr r) :
		handler_base(r)
	{
		log() << "Ctor Hello Xml handler\r\n";
	}
	
	virtual ~handler_hello_xml()
	{
		log() << "Dtor Hello Xml handler\r\n";
	}
	
	virtual bool parse(const char* begin, const char* end)
	{
		std::string str(begin, end);
		std::istringstream istr(str);
		istr >> doc_;
		
		return true;
	}
	
	virtual boost::tuple<bool, boost::any> react()
	{		
		TiXmlHandle dochandle(&doc_);
		
		TiXmlElement* greeting = dochandle.FirstChildElement("greeting").Element();
		if (greeting)
		{
			return boost::make_tuple(true, static_cast<std::string>(greeting->GetText()));
		}
		else
		{
			return boost::make_tuple(true, detail::void_class());
		}
	}
	
private:
	TiXmlDocument doc_;
	std::string reply_;
};

class handler_hello_xml_ctor :
	public handler_ctor
{
public:
	virtual handler_ptr create(response::structure_ptr r)
	{
		return handler_ptr(new response::handler_hello_xml(r));
	}
};

} // namespace response
} // namespace http

#endif // HTTP_HANDLER_RESPONSE_HELLO_XML
