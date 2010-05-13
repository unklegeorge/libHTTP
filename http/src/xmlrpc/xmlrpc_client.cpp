
#include <boost/format.hpp>

#include <http/xmlrpc/client.hpp>
#include <http/debug_logger.hpp>

namespace http
{
namespace client 
{
namespace xmlrpc
{
	
http::xmlrpc::param call(http::client::sync::connection_base_ptr con, 
	const boost::filesystem::path& uri, const std::string& method_name, 
	const http::xmlrpc::param_arguments& params, bool keep_alive)
{	

/*	static std::string text_xml = "\
<?xml version=\"1.0\" ?>\
<methodCall>\
    <methodName>method1</methodName>\
    <params>\
        <param>\
            <value>\
                <i4>123</i4>\
            </value>\
        </param>\
        <param>\
            <value>\
                <array>\
                    <data>\
                        <value>\
                            <string>Hello</string>\
                        </value>\
                        <value>\
                            <struct>\
                                <member>\
                                    <name>time</name>\
                                    <value>\
                                        <dateTime.iso8601>20060910T18:05:03</dateTime.iso8601>\
                                    </value>\
                                </member>\
                            </struct>\
                        </value>\
                        <value>\
                            <base64>aGVsbG8gdGhpcyBpcyBzb21lIGRhdGEgSSdkIGxpa2UgdG8gaGF2ZSBlbmNvZGVkIGludG8g&#x0A;YmFzZTY0&#x0A;</base64>\
                        </value>\
                    </data>\
                </array>\
            </value>\
        </param>\
    </params>\
</methodCall>";*/

	http::client::sync::post poster(con, uri, "1.1", 
		new http::response::xmlrpc::handler_ctor());
	
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	TiXmlElement* root = new TiXmlElement("methodCall");
	TiXmlElement* methodname = new TiXmlElement("methodName");
	TiXmlText* methodtext = new TiXmlText(method_name);
	
	methodname->LinkEndChild(methodtext);
	root->LinkEndChild(methodname);
	
	doc.LinkEndChild(decl);
	doc.LinkEndChild(root);
	
	if (!params.empty())
	{
		TiXmlElement* parameters = new TiXmlElement("params");
		root->LinkEndChild(parameters);
	
		for(http::xmlrpc::param_arguments::const_iterator it=params.begin(); it!=params.end(); ++it)
		{
			TiXmlElement* param = new TiXmlElement("param");
			TiXmlElement* value = new TiXmlElement("value");
			param->LinkEndChild(value);
			value->LinkEndChild(boost::apply_visitor(http::xmlrpc::param_to_xml(), *it));
			parameters->LinkEndChild(param);
		}
	}
	
	std::string xmldoc;
	xmldoc << doc;
	
	// doc.SaveFile("request.xml");
		
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
	boost::tie(result, ret_val) = poster.connection()->send_all_react(poster.vec());
	
	return boost::any_cast<http::xmlrpc::param>(ret_val);
}

} // namespace xmlrpc
} // namespace client
} // namespace http
