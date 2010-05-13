
#include <http/debug_logger.hpp>

namespace http
{

Logger<std::string, std::ostringstream>& log()
{
	static Logger<std::string, std::ostringstream> l;
	return l;
}

};
