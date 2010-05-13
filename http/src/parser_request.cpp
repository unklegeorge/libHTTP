
#include <string>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <http/server.hpp>
#include <http/handler_request_base.hpp>
#include <http/parser_request.hpp>
#include <http/parser_impl.hpp>
#include <http/debug_logger.hpp>

namespace http
{

namespace fs = boost::filesystem;

namespace request
{

parser::parser() :
	connection_base_(0)
{}

const server::connection_base& parser::connection() const
{
	if (connection_base_)
		return *connection_base_;
	else
		throw std::runtime_error("Connection Base ptr not set");
}

void parser::set_connection(server::connection_base* c)
{
	connection_base_ = c;
}

first_line::first_line()
{}

boost::statechart::result first_line::react(const ev_parse& evt)
{	
	const char* end_line = std::find(
		evt.begin(), evt.end(), '\n');
		
	if (end_line == evt.end())
	{		
		context<parser>().buffer().insert(
			context<parser>().buffer().end(),
			evt.begin(), evt.end()
			);
				
		return discard_event();
	}
	else
	{
		const char* new_line = end_line+1;
		
		std::string str(context<parser>().buffer().begin(),
			context<parser>().buffer().end());
			
		str += std::string(evt.begin(), new_line);		
		
		boost::regex reg("(\\w+)\\s+([\\w/]+)\\s+HTTP/([\\d\\.]+)\\s*");
		boost::smatch m;	
		context<parser>().buffer().clear();		
			
		if (boost::regex_match(str,m,reg))
		{
			fs::path uri(m[2]);
			
			context<parser>().structure().reset(new structure(m[1], uri, m[3]));
			
			log() << boost::format("%1% %2% HTTP/%3%\r\n") 
				% context<parser>().structure()->verb() 
				% m[2]
				% context<parser>().structure()->version();
			
			context<parser>().handler(
				context<parser>().connection().host().get_handler(
					uri, context<parser>().structure()));
			
			post_event(ev_parse(new_line, evt.end(), evt.result()));
			return transit<http::headers<parser, first_line> >();
		}
		else
		{
			throw(error("Bad request line." + str));	
			return discard_event();
		}
	}
}

unsigned parser::parse(const char* begin, const char* end)
{	
	unsigned result = 0;
	
	process_event(ev_parse(begin, end, result));

	return result;
}

} // namespace request

} // namespace http
