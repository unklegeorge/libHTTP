
#include <boost/format.hpp>

#include <tinyxml/tinyxml.h>

#include <http/hello/client.hpp>
#include <http/hello/handler_response_hello.hpp>
#include <http/hello/handler_response_hello_xml.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace client 
{

// All the logic for a handler is contained in two files. The first which implements 
// the handler itself and a second similar to this which describes the interface for 
// the client application. *** P.S. This need exception catches ***

std::string hello(http::client::sync::connection_base_ptr con, 
	const boost::filesystem::path& uri, std::string name, bool keep_alive = false)
{	
	// Create a request verb together with it request line as a struct
	http::client::sync::post poster(con, uri, "1.1", 
		new http::response::handler_hello_ctor());
		
	// Add headers to the request struct
	poster.req().add_header("Content-Type", "text");
	poster.req().add_header("Content-Length", 
		boost::lexical_cast<std::string>(name.size()));
		
	if (!keep_alive)
		poster.req().add_header("Connection", "close");
	
	// Reset and then create the vector of ASIO buffers to be sent
	poster.reset_buffers();
	poster.req().to_buffers(poster.vec());
	poster.append_to_buffers(boost::asio::buffer(name.c_str(), name.length()));
	
	log() << boost::format("Sending %1%\r\n") % poster.vec().size();
		
	bool result; boost::any ret_val;	
	boost::tie(result, ret_val) =  poster.connection()->send_all_react(poster.vec());
	
	if (result)
		return boost::any_cast<std::string>(ret_val);
	else
		return "ERROR!";
}

std::string hello_xml(http::client::sync::connection_base_ptr con, 
	const boost::filesystem::path& uri, std::string name, bool keep_alive = false)
{	
	// Create a request verb together with it request line as a struct
	http::client::sync::post poster(con, uri, "1.1", 
		new http::response::handler_hello_xml_ctor());
		
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	TiXmlElement* element = new TiXmlElement("greet");
	TiXmlText* text = new TiXmlText(name);
	
	element->LinkEndChild(text);
	doc.LinkEndChild(decl);
	doc.LinkEndChild(element);
	
	std::string xmldoc;
	xmldoc << doc;
		
	// Add headers to the request struct
	poster.req().add_header("Content-Type", "text/xml");
	poster.req().add_header("Content-Length", 
		boost::lexical_cast<std::string>(xmldoc.size()));
		
	if (!keep_alive)
		poster.req().add_header("Connection", "close");
	
	// Reset and then create the vector of ASIO buffers to be sent
	poster.reset_buffers();
	poster.req().to_buffers(poster.vec());
	poster.append_to_buffers(boost::asio::buffer(xmldoc.c_str(), xmldoc.length()));
	
	log() << boost::format("Sending %1%\r\n") % poster.vec().size();
		
	bool result; boost::any ret_val;	
	boost::tie(result, ret_val) =  poster.connection()->send_all_react(poster.vec());
	
	if (result)
		return boost::any_cast<std::string>(ret_val);
	else
		return "ERROR!";
}

} // namespace client
} // namespace http
