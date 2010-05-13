
#ifndef HTTP_DEBUG_LOG
#define HTTP_DEBUG_LOG

#include <string>
#include <sstream>

#include <boost/signals.hpp>
#include <boost/function.hpp>

namespace http 
{

template<class _string_type, class _stream_type>
class Logger
{
public:
	boost::signals::scoped_connection attach(boost::function<void (const _string_type& text)> fn)
	{
		return logger_.connect(fn);
	}
	
	inline Logger<_string_type, _stream_type>& operator<<(const _string_type& text)
	{
#		if defined(HTTP_DEBUG_LOGGING)
		logger_(text);
#		endif
		return *this;
	}

	template<class _type>
	inline Logger<_string_type, _stream_type>& operator<<(_type type)
	{
#		if defined(HTTP_DEBUG_LOGGING)
		_stream_type temp;
		temp << type;
		logger_(temp.str());
#		endif
		return *this;
	}

	friend Logger<std::string, std::ostringstream>& log();
	
private:
	Logger() {}
	Logger(const Logger& rhs) {}
	
	boost::signal<void (const _string_type& text)> logger_;
};

Logger<std::string, std::ostringstream>& log();

}; // namespace http

#endif // HTTP_DEBUG_LOG
