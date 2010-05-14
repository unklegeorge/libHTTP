
#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/path.hpp>

#include <http/xmlrpc/client.hpp>
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

int main()
{
	// This is an empty parameter vector
	http::xmlrpc::param_arguments params;
	
	// Here we can see how simple it is to add ints, strings or doubles
	params.push_back(1234);
	params.push_back("Hello");
	params.push_back(12.34);	
	
	// Now we'll create an array type
	http::xmlrpc::param_vec vec;
	
	// Adding element to the array is identical for the 
	// param_arguments except for the necessary "()"
	vec().push_back("World");
	
	// Creating a struct is also easy, its based on a map type
	http::xmlrpc::param_map map;
	
	// The time type is managed bay a boost date_time object
	ptime now = posix_time::second_clock::local_time();
	map()["time"] = now;

	// Here is how to encode a string into base64, but a vector of 
	// any binary data would work too
	string data = "Some data encoded into base64";
	http::xmlrpc::base64_data b64data;	
	copy(data.begin(), data.end(), back_inserter(b64data));
	map()["binary"] = b64data;
	
	// Here we add the struct to the array, and array to 
	// param_arguments
	vec().push_back(map);	
	params.push_back(vec);
	
	try
	{
	
	http::client::sync::connection_base_ptr 
		con(new http::client::sync::connection("127.0.0.1", 8081));
	
	http::xmlrpc::param result = http::client::xmlrpc::call(con ,"/xmlrpc", "RemoteMethod", params, true);
	
	boost::apply_visitor(print_visitor(), result);	
	
	}	
	catch (const http::xmlrpc::error& e)
	{
	cout << format("Fault code: %1%, string: %2%\r\n") % e.code() % e.str();
	}
	catch(std::exception& e)
	{
	cout << format("Exception: %1%.\r\n") % e.what();
	}
		
	cout << "Press enter to end application.\r\n";
	cin.get();
	
    return 0;
}
