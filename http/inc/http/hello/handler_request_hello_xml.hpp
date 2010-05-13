
#ifndef HTTP_HANDLER_REQUEST_HELLO_XML
#define HTTP_HANDLER_REQUEST_HELLO_XML

#include <string>
#include <sstream>

#include <tinyxml/tinyxml.h>

#include <http/handler_request_base.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace request
{

class handler_hello_xml :
	public handler_base
{
public:	
	handler_hello_xml(request::structure_ptr r,
		boost::filesystem::path base_uri, boost::filesystem::path relative_uri) :
			handler_base(r, base_uri, relative_uri)
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
	
	virtual std::pair<bool, std::vector<boost::asio::const_buffer> > react()
	{
		log() << "Responding\r\n";
		
		TiXmlHandle dochandle(&doc_);
		TiXmlElement* name = dochandle.FirstChildElement("greet").Element();
		if (name)
		{		
			std::string greeting = "Hello ";
			greeting += name->GetText();
			
			TiXmlDocument doc;
			TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
			TiXmlElement* element = new TiXmlElement("greeting");
			TiXmlText* text = new TiXmlText(greeting);
			
			element->LinkEndChild(text);
			doc.LinkEndChild(decl);
			doc.LinkEndChild(element);
			
			std::string xmldoc;
			xmldoc_ << doc;
			
			create_response(response::structure::ok);
			
			res().add_header("Content-Type", "text/xml");
			res().add_header("Content-Length", boost::lexical_cast<std::string>(xmldoc_.length()));
			
			reset_buffers();
			res().to_buffers(vec());
			
			append_to_buffers(boost::asio::const_buffer(xmldoc_.c_str(), xmldoc_.length()));
		}
		else
		{
			create_response(response::structure::bad_request);
			
			reset_buffers();
			res().to_buffers(vec());
		}
		
		return make_pair(true, vec());
	}
	
private:
	TiXmlDocument doc_;
	std::string xmldoc_;	
};

class handler_hello_xml_ctor :
	public handler_ctor
{
public:
	virtual handler_ptr create(request::structure_ptr r,
		boost::filesystem::path base_uri, boost::filesystem::path relative_uri)
	{
		return handler_ptr(new request::handler_hello_xml(r, base_uri, relative_uri));
	}
};

} // namespace request
} // namespace http

#endif // HTTP_HANDLER_REQUEST_HELLO_XML
