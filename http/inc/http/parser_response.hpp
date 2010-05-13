
#ifndef HTTP_PARSER_REQUEST
#define HTTP_PARSER_REQUEST

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>

#include <http/parser_base.hpp>
#include <http/structure_response.hpp>

namespace http
{

namespace client
{
namespace sync
{
class connection_base;
}
}

namespace response
{

class handler_base;
typedef boost::shared_ptr<handler_base> handler_ptr;

typedef std::map<std::string, std::string> string_map;
typedef std::map<std::string, std::string>::iterator string_map_it;
typedef std::map<std::string, std::string>::const_iterator string_map_cit;

struct first_line;

class parser : 
	public boost::statechart::state_machine<parser, first_line>,
	public parser_base
{
public:
	parser();
	~parser() {}
	
	unsigned parse(const char* begin, const char* end);
	
	void set_connection(client::sync::connection_base* c);
	const client::sync::connection_base& connection() const;
	
	response::structure_ptr& structure() { return structure_; }
	
	response::handler_ptr handler() { return handler_; }
	void handler(response::handler_ptr h) { handler_ = h; }
	
private:	
	const client::sync::connection_base* connection_base_;
	response::structure_ptr structure_;
	response::handler_ptr handler_;
};


struct first_line : boost::statechart::simple_state<first_line, parser> 
{
	typedef boost::statechart::custom_reaction<ev_parse> reactions;
	
	first_line();
	boost::statechart::result react(const ev_parse& evt);	
};

} // namespace response

} // namespace http

#endif // HTTP_PARSER_REQUEST
