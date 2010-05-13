
#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/path.hpp>

#include <http/xmlrpc/server.hpp>
#include "xmlrpc_print_visitor.hpp"

using namespace std;
using namespace boost;
using filesystem::path;
using posix_time::ptime;

// This class links the debug_logger provided by libHTTP to cout.
// It also prefixes all libHTTP messages with "http: ". The actual
// code for implementing the link is in the constructer.

static class debug_link
{
public:
	debug_link();
	void operator()(const string& text)
	{
		cout << "-> " << text;
	}
private:	
	signals::scoped_connection conn_;
	
} debug_link_;

debug_link::debug_link() :
	conn_(http::log().attach(debug_link_))
{}

// This is a sample remote method. The param vector is allowed to be empty.

http::xmlrpc::param remote_method(http::xmlrpc::param_arguments params)
{
	cout << "In Remote Method!\r\n";
	
	for(http::xmlrpc::param_arguments::const_iterator it=params.begin(); it!=params.end(); ++it)
	{
		apply_visitor(print_visitor(), *it);
	}

	// To return more than one param we must place them into a container type
	// either an array or struct.
	// See the client code for a better explanation of the different param types
	
	// Create an array type
	http::xmlrpc::param_vec vec;
	
	vec().push_back("Return string");
	vec().push_back(3.1415);
	
	return vec;
}

// Create a host listening on ports 80 for https and 443 for shttp (not implemented)
static http::host host;
	
// This is simply a seperate thread to run the host freeing up the main thread to 
// wait for the kill singnal.
void host_procedure()
{
	try
	{
	
	cout << "Running server...\r\n";
	
	host.bind_to(8081);
	host.run();

	cout << "Finished\r\n";
	
	}
	catch(const exception& e)
	{
	cout << format("Thread Error: %1%.\r\n") % e.what();
	}	
}

int main()
{
	try
	{
	
	http::xmlrpc::procedure_manager& man = http::server::add_xmlrpc_handler(host, "/xmlrpc");
	man.add_procedure("RemoteMethod", &remote_method);
	
	}
	catch(const exception& e)
	{
	cout << format("Main Error: %1%.\r\n") % e.what();
	}
	
	cout << "Initializing Host thread.\r\n";	
	thread host_thread(&host_procedure);
	
	cout << "Press enter to end application.\r\n";
	cin.get();
	
	host.stop();
	host_thread.join();
	
    return 0;
}

