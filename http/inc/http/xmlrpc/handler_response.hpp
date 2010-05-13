
#ifndef HTTP_HANDLER_RESPONSE_XML_RPC
#define HTTP_HANDLER_RESPONSE_XML_RPC

#include <string>

#include <tinyxml/tinyxml.h>

#include <http/handler_response_base.hpp>
#include <http/xmlrpc/xmlrpc.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace response
{
namespace xmlrpc
{

class handler:
	public handler_base
{
public:	
	handler(response::structure_ptr r) :
		handler_base(r)
	{
		log() << "Ctor Xml-Rpc handler\r\n";
	}
	
	virtual ~handler()
	{
		log() << "Dtor Xml-Rpc handler\r\n";
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
		if (doc_.Error())
		{
			throw http::xmlrpc::error(http::xmlrpc::failure::tinyxml_parse_error, doc_.ErrorDesc());
		}
		
		// doc_.SaveFile("response.xml");
		
		TiXmlHandle dochandle(&doc_);
		TiXmlNode* node;
		
		try
		{
		
		node = dochandle.FirstChildElement("methodResponse")
			.FirstChildElement("params").FirstChildElement("param").FirstChildElement("value")
			.FirstChildElement().Node();
		if (node)
		{				
			return boost::make_tuple(true, http::xmlrpc::xml_to_param(node));
		}
		
		node = dochandle.FirstChildElement("methodResponse")
			.FirstChildElement("fault").FirstChildElement("value").FirstChild().Node();
		if (node)
		{
			http::xmlrpc::param_map fault_map = boost::get<http::xmlrpc::param_map>
				(http::xmlrpc::xml_to_param(node));
				
			std::map<std::string, http::xmlrpc::param>::iterator result;
			int fault_code;
			std::string fault_string;
				
			result = fault_map().find("faultCode");				
			if (result != fault_map().end())
				fault_code = boost::get<int>((*result).second);
			else
				throw http::xmlrpc::error(http::xmlrpc::failure::xml_missing_fault_code, 
					"Fault Response missing faultCode");	
			
			result = fault_map().find("faultString");				
			if (result != fault_map().end())
				fault_string = boost::get<std::string>((*result).second);
			else
				throw http::xmlrpc::error(http::xmlrpc::failure::xml_missing_fault_string, 
					"Fault Response missing faultString");
			
			// server_code_offset?? its just a modifier so that back at the call site parser 
			// errors generated when decoding the request can be distinguished from those 
			// generated when decoding the response.
			throw http::xmlrpc::error(fault_code + http::xmlrpc::failure::server_code_offset, 
				fault_string);
		}
		
		throw http::xmlrpc::error(http::xmlrpc::failure::bad_response, "Bad response");
		
		}
		catch (const http::xmlrpc::error& e)
		{
			throw e;
		}
		catch (const std::exception& e)
		{
			throw http::xmlrpc::error(http::xmlrpc::failure::exception_caught, e.what());
		}
	}
	
private:
	TiXmlDocument doc_;
};

class handler_ctor :
	public response::handler_ctor
{
public:
	virtual handler_ptr create(response::structure_ptr r)
	{
		return handler_ptr(new response::xmlrpc::handler(r));
	}
};

} // namespace xmlrpc
} // namespace response
} // namespace http

#endif // HTTP_HANDLER_RESPONSE_XML_RPC
