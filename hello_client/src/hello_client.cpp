
#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/path.hpp>

using namespace std;
using namespace boost;
using filesystem::path;

#include <http/hello/client.hpp>

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
	try
	{
	
	// asio::ssl::context::sslv23
	
	// A connection object is created and managed by a shared_ptr. No
	// attempt is made to connect to a server yet.	
	http::client::sync::connection_base_ptr
		con(new http::client::sync::connection("127.0.0.1", 8080));
		
	// Here we use it twice, on on the second use we pass false to 
	// keep-alive telling the server to disconnect the socket.	
    cout << http::client::hello(con, "/hello", "World!", true) << "\r\n";
    cout << http::client::hello(con, "/hello", "World Again!", false) << "\r\n";

#	if defined(HTTP_SUPPORT_OPENSSL)
	// Do it all again with an SSL connection.
	http::client::sync::ssl_connection_ptr
		ssl_con(new http::client::sync::ssl_connection("127.0.0.1", 8443, http::boost__asio::ssl::context::sslv23));

	http::boost__asio::ssl::context& context = ssl_con->context();
	
	context.set_verify_mode(http::boost__asio::ssl::context::verify_peer);
	context.load_verify_file("server.crt");
	
    cout << http::client::hello(ssl_con, "/hello", "Secured World!", false) << "\r\n";
#	endif

	}
	catch(const std::exception& e)
	{
	cout << format("Main Error: %1%.\r\n") % e.what();
	}
	
	cout << "Press enter to end application.\r\n";
	cin.get();
	
    return 0;
}
