
#include <string>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <http/client_verb.hpp>
#include <http/handler_response_base.hpp>
#include <http/parser_response.hpp>
#include <http/parser_impl.hpp>
#include <http/client_connection_base.hpp>
#include <http/debug_logger.hpp>

namespace http
{

namespace fs = boost::filesystem;

namespace response
{

parser::parser() :
	connection_base_(0)
{}

const client::sync::connection_base& parser::connection() const
{
	if (connection_base_)
		return *connection_base_;
	else
		throw std::runtime_error("Connection Base ptr not set");
}

void parser::set_connection(client::sync::connection_base* c)
{
	connection_base_ = c;
}

first_line::first_line()
{}

boost::statechart::result first_line::react(const ev_parse& evt)
{	
	const char* end_line = std::find(evt.begin(), evt.end(), '\n');
	
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
		
		boost::regex reg("HTTP/([\\d\\.]+)\\s+(\\d+)\\s+([\\w\\s]+?)\\s*");
		boost::smatch m;	
		context<parser>().buffer().clear();		
		
		if (boost::regex_match(str,m,reg))
		{
			context<parser>().structure().reset(new structure(m[2], m[3], m[1]));
			
			log() << boost::format("HTTP/%1% %2% %3%\r\n") 
				% m[1]
				% m[2]
				% m[3]; 
				
			if (boost::lexical_cast<unsigned>(m[2]) != 200)
				throw error((boost::format("HTTP Error %1%") % m[2]).str());
				
			context<parser>().handler(
				context<parser>().connection().verb().get_handler()->
					create(context<parser>().structure()));
							
			post_event(ev_parse(new_line, evt.end(), evt.result()));
			return transit<http::headers<parser, first_line> >();
		}
		else
		{
			throw(error("Bad response line."));	
			return discard_event();
		}
	}
}

unsigned parser::parse(const char* begin, const char* end)
{

/*	size_t buffer_len = end - begin;
	wlog() << wformat(L">> %1%\r\n") % buffer_len;
	
	std::string str(begin, end);
	wlog() << global_log::mbstowcs(str) << L"\r\n";
*/	
	unsigned result = 0;

	process_event(ev_parse(begin, end, result));

	return result;
}

} // namespace response

} // namespace http
