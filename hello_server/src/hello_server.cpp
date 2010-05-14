
#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/path.hpp>

using namespace std;
using namespace boost;
using filesystem::path;
#include <http/hello/server.hpp>

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

// Here we create the HTTP host, at this point the server binds to 
// the two ports but cannot react to any incomming connections until 
// it is "run".

static http::host host;

#if defined(HTTP_SUPPORT_OPENSSL)
std::string get_passwords(size_t, http::boost__asio::ssl::context_base::password_purpose)
{
	return "eoin";
}
#endif
	
void host_procedure()
{
	try
	{
	
	cout << "Running server...\r\n";
	
	host.bind_to(8080);
	
#	if defined(HTTP_SUPPORT_OPENSSL)
	http::boost__asio::ssl::context& context = 
		host.prepare_ssl_context(http::boost__asio::ssl::context::sslv23);
	
	context.set_options(
		http::boost__asio::ssl::context::default_workarounds | 
		http::boost__asio::ssl::context::no_sslv2);
		
	context.set_password_callback(&get_passwords);
	context.use_certificate_chain_file("server.crt");
	context.use_private_key_file("server.pem", http::boost__asio::ssl::context::pem);
	
	host.ssl_bind_to(8443);
#	endif

	host.run();

	cout << "Finished\r\n";
	
	}
	catch(const std::exception& e)
	{
	cout << format("Thread Error: %1%.\r\n") % e.what();
	}	
}

int main()
{
	try
	{
	
	http::server::add_hello_handler(host, path("/hello"));
	
	}
	catch(const std::exception& e)
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
