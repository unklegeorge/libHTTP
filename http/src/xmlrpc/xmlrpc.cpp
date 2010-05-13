
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/format.hpp>

#include <tinyxml/tinyxml.h>
#include <b64/decode.h>

#include <http/xmlrpc/xmlrpc.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace xmlrpc
{

param xml_to_param(TiXmlNode* node)
{	
	try
	{
	
	if(!node) throw error(failure::xml_empty_node, "Empty XML Node sent to parser");
	
	// This returns a sting when there are no "type" tags.
	TiXmlText* text = node->ToText();
	if (text)
	{
		log() << text->Value();
		return static_cast<std::string>(text->Value());
	}
	
	TiXmlElement* elem = node->ToElement();
	if (!elem) throw error(failure::xml_invalid_node, "XML Node not valid");
	
	std::string name = boost::to_lower_copy<std::string>(elem->Value());
	
	if (name == "i4" || name == "int")
	{		
		const char* text = elem->GetText();
		if (text)
			return boost::lexical_cast<int>(text);
		else
			throw error(failure::xml_bad_int, "Bad XML int or i4 type");
	}
	else if (name == "double")
	{		
		const char* text = elem->GetText();
		if (text)
			return boost::lexical_cast<double>(text);
		else
			throw error(failure::xml_bad_double, "Bad XML double type");
	}
	else if (name == "string")
	{
		const char* text = elem->GetText();
		if (text)
			return static_cast<std::string>(text);
		else
			throw error(failure::xml_bad_string, "Bad XML string type");	
	}
	else if (name == "base64")
	{
		const char* text = elem->GetText();
		if (text)
		{
			std::vector<char> b64data;
			std::istringstream isstr(text);
			base64::decoder dec;
		
			dec.decode(isstr, b64data);
			return static_cast<std::vector<char> >(b64data);
		}
		else
			throw error(failure::xml_bad_base64, "Bad XML base64 type");	
	}
	else if (name == "array")
	{
		TiXmlNode* data = elem->FirstChild("data");
		if (data) 
		{
			TiXmlNode* value = data->FirstChild("value");
			param_vec vec;
			
			while(value)
			{
				vec().push_back(xml_to_param(value->FirstChild()));
				value = value->NextSibling();
			} 
			
			return vec;
		}
		throw error(failure::xml_bad_array, "Bad XML array type");
	}
	else if (name == "struct")
	{
		TiXmlNode* member = elem->FirstChild("member");
		param_map map;
		
		while(member)
		{
			TiXmlElement* member_name = static_cast<TiXmlElement*>(member->FirstChild("name"));
			std::string name;
			
			if (member_name)
				name = member_name->GetText();
			else
				throw error(failure::xml_bad_struct, "Bad XML struct name");
			
			TiXmlElement* value = static_cast<TiXmlElement*>(member->FirstChild("value"));
			if (value)			
				map()[name] = xml_to_param(value->FirstChild());
			else
				throw error(failure::xml_bad_struct, "Bad XML struct value");
				
			member = member->NextSibling();
		} 		
		return map; 
	}
	else if (name == "datetime.iso8601")
	{
		const char* text = elem->GetText();
		if (text)
		{
			std::istringstream sstr(text);
			boost::posix_time::time_input_facet* facet = new boost::posix_time::time_input_facet("%Y%m%dT%H:%M:%S");
			
			sstr.imbue(std::locale(std::cout.getloc(), facet));
			sstr.exceptions(std::ios_base::failbit);
			
			boost::posix_time::ptime tim;			
			try
			{
				sstr >> tim;
			}
			catch (...)
			{
				throw error(failure::xml_bad_datetime, "Bad XML dateTime type parsed");
			}			
			return tim;
		}
		else
			throw error(failure::xml_bad_datetime, "Bad XML dateTime type");
	}
	else
	{		
		throw error(failure::xml_unknown_type, (boost::format("XML unknown type: %1%") % name).str());
	}	
	
	}
	catch (const error& e)
	{
		throw e;
	}
	catch (const std::exception& e)
	{
		error(failure::xml_exception_caught, e.what());
	}
	// Was null return
	param_vec vec;
	return vec;
}	

} // namespace xmlrpc
} // namespace http

