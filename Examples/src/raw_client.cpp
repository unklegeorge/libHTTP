
#include <iostream>
#include <iomanip>

#include <boost/thread/thread.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/path.hpp>

using namespace std;
using namespace boost;
using filesystem::path;

#include <http/client.hpp>
using asio::ip::tcp;
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
	// A connection object is created and managed by a shared_ptr. No
	// attempt is made to connect to a server yet.	
	http::client::sync::connection_ptr
		connection(new http::client::sync::connection("127.0.0.1", 8080));
	connection->connect();
	
	http::boost__asio::error_code error;
	
	ifstream ifs("raw.txt", ios::binary);
	boost::array<char, 1024> buf;
	
	while (!ifs.eof())
	{
		ifs.getline(buf.data(), buf.size(), '*');
		
		http::boost__asio::write(connection->socket(),
			http::boost__asio::buffer(buf.data(), strlen(buf.data())),
			http::boost__asio::transfer_all(), error);
		
		if (error)
			throw http::boost__asio::system_error(error);
		
		Sleep(50);
	}
	
	for (;;)
    {
		size_t len = connection->socket().read_some(asio::buffer(buf), error);
		
		if (error == asio::error::eof)
			break;
		else if (error)
			throw asio::system_error(error);
		
		std::cout.write(buf.data(), len);
    }
	
	}
	catch(const exception& e)
	{
	cout << format("Main Error: %1%.\r\n") % e.what();
	}
	
	cout << "Press enter to end application.\r\n";
	cin.get();
	
    return 0;
}
