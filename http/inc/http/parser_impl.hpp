
#ifndef HTTP_PARSER_IMPL
#define HTTP_PARSER_IMPL

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

template<class P, class F>
struct headers : boost::statechart::simple_state<headers<P, F>, P> 
{
	typedef typename boost::statechart::simple_state<headers<P, F>, P> base_type;
	typedef boost::statechart::custom_reaction<ev_parse> reactions;
	
	headers() {}
	virtual ~headers() {}
	
	boost::statechart::result react(const ev_parse& evt);	
	boost::statechart::result interpret_header(const ev_parse& evt);
};

template<class P, class F>
struct content : boost::statechart::state<content<P, F>, P> 
{
	typedef typename boost::statechart::state<content<P, F>, P> base_type;
	typedef boost::statechart::custom_reaction<ev_parse> reactions;
	
	content(typename base_type::my_context ctx) :
		base_type::my_base(ctx),
		content_length_(0)
	{ 
		initiate_transaction(); 
	}
	virtual ~content() {}
	
	boost::statechart::result react(const ev_parse& evt);
	
	void initiate_transaction();
	void complete_transaction(const ev_parse& evt);

	size_t& content_length() { return content_length_; }
	size_t content_length() const { return content_length_; }
	
private:
	size_t content_length_;	
};

template<class P, class F>
struct chunked_size;

template<class P, class F>
struct chunked : boost::statechart::state<chunked<P, F>, P, boost::mpl::list< chunked_size<P, F> > > 
{	
	typedef typename boost::statechart::state<chunked<P, F>, P, boost::mpl::list< chunked_size<P, F> > > base_type;
	
	chunked(typename base_type::my_context ctx) :
		base_type::my_base(ctx),
		content_length_(0)
	{}	
	virtual ~chunked() {}
	
	size_t& content_length() { return content_length_; }
	size_t content_length() const { return content_length_; }
	
private:
	size_t content_length_;
};

template<class P, class F>
struct chunked_size : boost::statechart::state<chunked_size<P, F>, chunked<P, F> > 
{
	typedef typename boost::statechart::state<chunked_size<P, F>, chunked<P, F> > base_type;
	typedef boost::statechart::custom_reaction<ev_parse> reactions;
	
	chunked_size(typename base_type::my_context ctx) :
		base_type::my_base(ctx)
	{}	
	virtual ~chunked_size() {}
	
	boost::statechart::result react(const ev_parse& evt);
};

template<class P, class F>
struct chunked_content : boost::statechart::state<chunked_content<P, F>, chunked<P, F> > 
{
	typedef typename boost::statechart::state<chunked_content<P, F>, chunked<P, F> > base_type;
	typedef boost::statechart::custom_reaction<ev_parse> reactions;
	
	chunked_content(typename base_type::my_context ctx) :
		base_type::my_base(ctx)
	{}
	virtual ~chunked_content() {}
	
	size_t& content_length() { return content_length_; }
	size_t content_length() const { return content_length_; }
	
	boost::statechart::result react(const ev_parse& evt);
	
private:
	size_t content_length_;	
};

template<class P, class F>
struct chunked_headers : boost::statechart::state<chunked_headers<P, F>, chunked<P, F> > 
{
	typedef typename boost::statechart::state<chunked_headers<P, F>, chunked<P, F> > base_type;
	typedef boost::statechart::custom_reaction<ev_parse> reactions;
	
	chunked_headers(typename base_type::my_context ctx) :
		base_type::my_base(ctx)
	{}
	virtual ~chunked_headers() {}
	
	boost::statechart::result react(const ev_parse& evt);
	void complete_transaction(const ev_parse& evt);
};

template<class P, class F>
boost::statechart::result headers<P, F>::interpret_header(const ev_parse& evt)
{	
	// At this stage we've finished processing headers (except any that 
	// may come after a chunked-encoding transfer)
	
	bool result; std::string value;
	
	boost::tie(result, value)= base_type::template 
		context<P>().structure()->get_value("transfer-encoding");
	
	if (result)
	{
		boost::to_lower(value);		
		if (value == "chunked")
			return base_type::template transit<chunked<P, F> >();
	}
	
	boost::tie(result, boost::tuples::ignore)= base_type::template 
		context<P>().structure()->get_value("content-type");
	
	if (result)			
		return base_type::template transit<content<P, F> >();	
	else
	{
		base_type::template context<P>().connection().handler_queue().add_handler(
			base_type::template context<P>().handler());
		base_type::template context<P>().handler().reset();
		
		boost::tie(result, value) = base_type::template context<P>().structure()
			->get_value("connection");
		if (result)
		{		
			if (value == "close") 
			{
				evt.set_close_connection();	
				base_type::template context<P>().handler()->set_close_connection();
			}
		}
		evt.set_completed_transaction();
		return base_type::template transit<F>();	
	}
}

template<class P, class F>
boost::statechart::result headers<P, F>::react(const ev_parse& evt)
{			
	const char* end_line = std::find(evt.begin(), evt.end(), '\n');
	
	if (end_line == evt.end())
	{		
		base_type::template context<P>().buffer().insert(
			base_type::template context<P>().buffer().end(), evt.begin(), evt.end() );
		
		return base_type::discard_event();
	}
	else
	{
		const char* new_line = end_line+1;
		
		std::string str(base_type::template context<P>().buffer().begin(),
			base_type::template context<P>().buffer().end());			
		str += std::string(evt.begin(), new_line);
		
		boost::regex reg_new_line("[\\r\\n]+");
		
		base_type::template context<P>().buffer().clear();
		base_type::post_event(ev_parse(new_line, evt.end(), evt.result()));	
		
		if (boost::regex_match(str, reg_new_line))
		{
			return interpret_header(evt);
		}
		
		boost::regex reg_begin_space("\\s+(.+?)\\s*");
		boost::smatch m_begin_space;
		
		if (boost::regex_match(str, m_begin_space, reg_begin_space))
		{
			if (!base_type::template context<P>().structure()->append_header(m_begin_space[1]))
				throw error("Bad continued header.");		
			
			log() << boost::format(" %1%\r\n") % m_begin_space[1];	
			return base_type::discard_event();	
		}
		
		boost::regex reg("(.+?):\\s*(.+?)\\s*");
		boost::smatch m;
		
		if (boost::regex_match(str, m, reg))
		{		
			base_type::template context<P>().structure()->add_header(m[1], m[2]);
			
			log() << boost::format("%1%:%2%\r\n") % m[1] % m[2];
			return base_type::discard_event();
		}
		else
			throw error("Bad header.");		
	}
}

template<class P, class F>
void content<P, F>::initiate_transaction()
{	
	bool result; std::string length;
	boost::tie(result, length) = base_type::template context<P>().structure()
		->get_value("content-length");
	
	if (result)
		content_length() = boost::lexical_cast<size_t>(length);
	else
		content_length() = 0;
}
	
template<class P, class F>
void content<P, F>::complete_transaction(const ev_parse& evt)
{	
	bool result; std::string value;
	boost::tie(result, value) = base_type::template context<P>().structure()
		->get_value("connection");
	if (result)
	{		
		if (value == "close") 
		{
			evt.set_close_connection();	
			base_type::template context<P>().handler()->set_close_connection();
		}
	}
	
	evt.set_completed_transaction();
	
	base_type::template context<P>().connection().handler_queue().add_handler(
		base_type::template context<P>().handler());
	base_type::template context<P>().handler().reset();
}

template<class P, class F>
boost::statechart::result content<P, F>::react(const ev_parse& evt)
{
	size_t lenght = content_length();
	size_t buffer_len = evt.end() - evt.begin();
	
	if (buffer_len > lenght)
	{
		if (lenght != 0)
		{
			content_length() = 0;			
			base_type::template context<P>().handler()->
				parse(evt.begin(), evt.begin() + lenght);
		}
		base_type::post_event(ev_parse(evt.begin() + lenght, evt.end(), evt.result()));	
	}
	else if (buffer_len == lenght)
	{
		content_length() = 0;
		base_type::template context<P>().handler()->parse(evt.begin(), evt.end());
	}
	else
	{
		content_length() = lenght - buffer_len;
		base_type::template context<P>().handler()->parse(evt.begin(), evt.end());
		
		return base_type::discard_event();	
	}	
	
	complete_transaction(evt);
	return base_type::template transit<F>();	
}

template<class P, class F>
boost::statechart::result chunked_size<P, F>::react(const ev_parse& evt)
{
	const char* end_line = std::find(evt.begin(), evt.end(), '\n');
	
	if (end_line == evt.end())
	{		
		base_type::template context<P>().buffer().insert(
			base_type::template context<P>().buffer().end(), evt.begin(), evt.end());
	}
	else
	{
		const char* new_line = end_line+1;
		
		std::string str(base_type::template context<P>().buffer().begin(),
			base_type::template context<P>().buffer().end());			
		str += std::string(evt.begin(), new_line);
		
		base_type::template context<P>().buffer().clear();
		base_type::post_event(ev_parse(new_line, evt.end(), evt.result()));
		
		boost::regex reg_new_line("[\\r\\n]+");
		
		if (boost::regex_match(str, reg_new_line))
		{
			return base_type::discard_event();
		}
		
		boost::regex reg_zero_line("0\\s*");
		
		if (boost::regex_match(str, reg_zero_line))
		{
			return base_type::template transit<chunked_headers<P, F> >();
		}

		boost::regex reg("\\s*([0-9a-fA-F]+);(.*?)\\s*");
		boost::smatch m;			
		
		if (boost::regex_match(str, m, reg))
		{		
			log() << boost::format("%1%;%2%\r\n") % m[1] % m[2];
			
			std::stringstream str(m[1]);
			str << std::setbase(16);			
			str >> base_type::template context<chunked<P,F> >().content_length();
			
			return base_type::template transit<chunked_content<P, F> >();
		}
		else
			throw error("Bad chunked size.");	
	}
	
	return base_type::discard_event();
}

template<class P, class F>
boost::statechart::result chunked_content<P, F>::react(const ev_parse& evt)
{
	size_t length = base_type::template context<chunked<P,F> >().content_length();
	size_t buffer_len = evt.end() - evt.begin();
	
	std::string str(evt.begin(), evt.end());
	
	if (buffer_len > length)
	{
		if (length != 0)
		{
			base_type::template context<chunked<P,F> >().content_length() = 0;			
			base_type::template context<P>().handler()->
				parse(evt.begin(), evt.begin() + length);
		}
		base_type::post_event(ev_parse(evt.begin() + length, evt.end(), evt.result()));	
	}
	else if (buffer_len == length)
	{
		base_type::template context<chunked<P,F> >().content_length() = 0;
		base_type::template context<P>().handler()->parse(evt.begin(), evt.end());
	}
	else
	{
		base_type::template context<chunked<P,F> >().content_length() = length - buffer_len;
		base_type::template context<P>().handler()->parse(evt.begin(), evt.end());
		
		return base_type::discard_event();	
	}	
	
	return base_type::template transit<chunked_size<P, F> >();	
}

template<class P, class F>
void chunked_headers<P, F>::complete_transaction(const ev_parse& evt)
{	
	bool result; std::string value;
	boost::tie(result, value) = base_type::template context<P>().structure()
		->get_value("connection");
	if (result)
	{		
		if (value == "close") 
		{
			evt.set_close_connection();	
			base_type::template context<P>().handler()->set_close_connection();
		}
	}
	
	evt.set_completed_transaction();
	
	base_type::template context<P>().connection().handler_queue().add_handler(
		base_type::template context<P>().handler());
	base_type::template context<P>().handler().reset();
}

template<class P, class F>
boost::statechart::result chunked_headers<P, F>::react(const ev_parse& evt)
{
	const char* end_line = std::find(evt.begin(), evt.end(), '\n');
	
	if (end_line == evt.end())
	{		
		base_type::template context<P>().buffer().insert(
			base_type::template context<P>().buffer().end(), evt.begin(), evt.end() );
		
		return base_type::discard_event();
	}
	else
	{
		const char* new_line = end_line+1;
		
		std::string str(base_type::template context<P>().buffer().begin(),
			base_type::template context<P>().buffer().end());			
		str += std::string(evt.begin(), new_line);
		
		base_type::template context<P>().buffer().clear();
		base_type::post_event(ev_parse(new_line, evt.end(), evt.result()));	
		
		boost::regex reg_new_line("[\\r\\n]+");
		
		if (boost::regex_match(str, reg_new_line))
		{
			complete_transaction(evt);
			return base_type::template transit<F>();
		}
		
		boost::regex reg_begin_space("\\s+(.+?)\\s*");
		boost::smatch m_begin_space;
		
		if (boost::regex_match(str, m_begin_space, reg_begin_space))
		{
			if (!base_type::template context<P>().structure()->append_header(m_begin_space[1]))
				throw error("Bad continued header.");		
			
			log() << boost::format(" %1%\r\n") % m_begin_space[1];
			return base_type::discard_event();
		}
		
		boost::regex reg("(.+?):\\s*(.+?)\\s*");
		boost::smatch m;
		
		if (boost::regex_match(str, m, reg))
		{		
			base_type::template context<P>().structure()->add_header(m[1], m[2]);
			
			log() << boost::format("%1%:%2%\r\n") % m[1] % m[2];
			return base_type::discard_event();
		}
		else
			throw error("Bad header.");		
	}
}

} // namespace http

#endif // HTTP_PARSER_IMPL
