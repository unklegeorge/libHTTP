
#ifndef HTTP_HANDLER_REQUEST_XMLRPC
#define HTTP_HANDLER_REQUEST_XMLRPC

#include <string>
#include <sstream>

#include <tinyxml/tinyxml.h>

#include <http/handler_request_base.hpp>
#include <http/xmlrpc/xmlrpc.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace request
{
namespace xmlrpc
{

class handler :
	public handler_base
{
public:	
	handler(request::structure_ptr r,
		boost::filesystem::path base_uri, boost::filesystem::path relative_uri,
		http::xmlrpc::procedure_manager& manager) :
			handler_base(r, base_uri, relative_uri),
			manager_(manager)
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
	
	void generate_fault_response(int fault_code, std::string fault_text)
	{
		TiXmlDocument doc;
		TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
		TiXmlElement* root = new TiXmlElement("methodResponse");
		TiXmlElement* fault = new TiXmlElement("fault");
		
		root->LinkEndChild(fault);
		
		doc.LinkEndChild(decl);
		doc.LinkEndChild(root);
		
		http::xmlrpc::param_map fault_struct;
		fault_struct()["faultCode"] = fault_code;
		fault_struct()["faultString"] = fault_text;
		
		http::xmlrpc::param parameter = fault_struct; // MinGW bug workaround
	   
		TiXmlElement* value = new TiXmlElement("value");
		value->LinkEndChild(boost::apply_visitor(http::xmlrpc::param_to_xml(), parameter));
		fault->LinkEndChild(value);
		
		xmldoc_ << doc;
		
		// doc.SaveFile("fault.xml");
		
		create_response(response::structure::ok);
		
		res().add_header("Content-Type", "text/xml");
		res().add_header("Content-Length", boost::lexical_cast<std::string>(xmldoc_.length()));
		
		reset_buffers();
		res().to_buffers(vec());
		
		append_to_buffers(boost::asio::const_buffer(xmldoc_.c_str(), xmldoc_.length()));
	}
		
	virtual std::pair<bool, std::vector<boost::asio::const_buffer> > react()
	{
		log() << "Responding\r\n";
		
		try
		{
		
		if (doc_.Error())
		{
			throw http::xmlrpc::error(http::xmlrpc::failure::tinyxml_parse_error, doc_.ErrorDesc());
		}
		
		TiXmlHandle dochandle(&doc_);
		TiXmlNode* params = dochandle.FirstChildElement("methodCall")
			.FirstChildElement("params").Node();
		
		http::xmlrpc::param_arguments p_vec;
		
		if (params)
		{
			TiXmlNode* param = params->FirstChild("param");
			while (param) 
			{
				TiXmlNode* value = param->FirstChild("value");
				if (value)
				{
					p_vec.push_back(http::xmlrpc::xml_to_param(value->FirstChild()));
				}
				param = param->NextSibling();
			}
		}
		else
		{
		//  Parameterless calls are allowed
		//	throw http::xmlrpc::error(http::xmlrpc::failure::bad_request_parameter,"Bad parameter XML");
		}
		
		TiXmlElement* methodname = dochandle.FirstChildElement("methodCall")
			.FirstChildElement("methodName").Element();
		
		if (methodname)
		{			
			http::xmlrpc::procedure proc = manager_.get_procedure(methodname->GetText());
			http::xmlrpc::param result;
			
			if (!proc.empty())
			{
				result = proc(p_vec);
			}
			else
			{
				throw http::xmlrpc::error(http::xmlrpc::failure::no_matching_method,
					"No matching Method found");
			}
			
			TiXmlDocument doc;
			TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
			TiXmlElement* root = new TiXmlElement("methodResponse");
			TiXmlElement* parameters = new TiXmlElement("params");
			
			root->LinkEndChild(parameters);
			
			doc.LinkEndChild(decl);
			doc.LinkEndChild(root);
			
			TiXmlElement* param = new TiXmlElement("param");
			TiXmlElement* value = new TiXmlElement("value");
			param->LinkEndChild(value);
			value->LinkEndChild(boost::apply_visitor(http::xmlrpc::param_to_xml(), result));
			parameters->LinkEndChild(param);
			
			xmldoc_ << doc;
			
		//	doc.SaveFile("response.xml");
			
			create_response(response::structure::ok);
			
			res().add_header("Content-Type", "text/xml");
			res().add_header("Content-Length", boost::lexical_cast<std::string>(xmldoc_.length()));
			
			reset_buffers();
			res().to_buffers(vec());
			
			append_to_buffers(boost::asio::const_buffer(xmldoc_.c_str(), xmldoc_.length()));
		}
		else
		{
			throw http::xmlrpc::error(http::xmlrpc::failure::bad_request_method_name,
				"Missing or malformed Method Name");
		}

		}
		catch(const http::xmlrpc::error& e)
		{
			generate_fault_response(e.code(), e.str());
		}
		catch(const std::exception& e)
		{
			generate_fault_response(http::xmlrpc::failure::exception_caught, e.what());
		}
		
		return std::make_pair(true, vec());		
	}
	
private:
	TiXmlDocument doc_;
	std::string xmldoc_;
	http::xmlrpc::procedure_manager& manager_;
};

class handler_ctor :
	public request::handler_ctor
{
public:
	virtual handler_ptr create(request::structure_ptr r,
		boost::filesystem::path base_uri, boost::filesystem::path relative_uri)
	{
		return handler_ptr(new request::xmlrpc::handler(r, base_uri, relative_uri, manager_));
	}
	http::xmlrpc::procedure_manager& manager() { return manager_; }
	
private:
	http::xmlrpc::procedure_manager manager_;
};

typedef boost::shared_ptr<handler_ctor> handler_ctor_ptr;

} // namespace xmlrpc
} // namespace request
} // namespace http

#endif // HTTP_HANDLER_REQUEST_XMLRPC
