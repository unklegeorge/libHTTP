
#ifndef HTTP_PARSER_BASE
#define HTTP_PARSER_BASE

#include <vector>
#include <stdexcept>
#include <iomanip>

#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>

#include <http/debug_logger.hpp>

namespace http
{

class error : public std::exception
{
public:
	error(const std::string& message) :
		str_(message)
	{}
	virtual ~error() throw () {}
	
	virtual const char *what() const throw () 
	{
		return (str_.c_str());
	}	
	
private:
	std::string str_;
};

struct ev_parse : boost::statechart::event<ev_parse>
{
public:
	ev_parse(const char* begin, const char* end, unsigned& result) : 
		begin_(begin),
		end_(end), 
		result_(result)
	{}
	
	enum
	{
		close_connection = 0x1,
		completed_transaction = 0x2
	};
	
	const char* begin() const { return begin_; }
	const char*& begin() { return begin_; }
	const char* end() const { return end_; }
	const char*& end() { return end_; }	
	
	unsigned& result() const { return result_; }

	void set_close_connection() const { result_ |= ev_parse::close_connection; }
	void set_completed_transaction() const { result_ |= ev_parse::completed_transaction; }

private:
	const char* begin_;
	const char* end_;
	mutable unsigned& result_;
};

class parser_base
{
public:	
	parser_base() {}
	
	bool parse(const char* begin, const char* end);
	
	std::vector<char>& buffer() { return buffer_; }
	const unsigned get_last_error() const { return error_; }	

protected:
	virtual ~parser_base() {}
	
	unsigned error_;
	std::vector<char> buffer_;	
};

} // namespace http

#endif // HTTP_PARSER_BASE
