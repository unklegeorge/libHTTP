#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <iostream>
#include <string>
#include <ctime>


namespace fs = boost::filesystem;
namespace io = boost::asio;
using boost::asio::ip::tcp;

std::string make_daytime_string() {
	using namespace std;
	time_t now = time(0);
	return ctime(&now);
}

class tcp_connection :public boost::enable_shared_from_this<tcp_connection> {
	tcp::socket d_socket;
	std::string d_message;
public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	static pointer create(io::io_service &io_service) {
		return pointer(new tcp_connection(io_service));
	}

	tcp::socket& socket() {
		return d_socket;
	}

	void start() {
		d_message = make_daytime_string();
		io::async_write(d_socket, io::buffer(d_message),
				boost::bind(&tcp_connection::handle_write, shared_from_this()));
	}

private:
	tcp_connection(io::io_service& io_service)
		: d_socket(io_service) {}

	void handle_write() {}
};

class tcp_server {
	tcp::acceptor d_acceptor;

public:
	tcp_server(io::io_service &io_service)
		: d_acceptor(io_service, tcp::endpoint(tcp::v4(), 8181)) {
		start_accept();
	}

private:
	void start_accept() {
		tcp_connection::pointer new_connection = tcp_connection::create(d_acceptor.io_service());
		d_acceptor.async_accept(new_connection->socket(),
				boost::bind(&tcp_server::handle_accept, this, new_connection,
				io::placeholders::error));
	}

	void handle_accept(tcp_connection::pointer new_connection,
			const boost::system::error_code &error)	{
		if ( !error ) {
			new_connection->start();
			start_accept();
		}
	}
};

int main(int argc, char* argv[]) {

	try {
		io::io_service io_service;
		tcp_server server(io_service);
		io_service.run();
  } catch ( std::exception &e ) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}


#if 0
int main(int argc, char* argv[]) {

	try {
		io::io_service io_service;
		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8181));

		for(;;) {
			tcp::socket socket(io_service);
			acceptor.accept(socket);

			std::string message = make_daytime_string();
			boost::system::error_code ignored_error;
			io::write(socket, io::buffer(message),
					io::transfer_all(), ignored_error);
		}

  } catch ( std::exception &e ) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
#endif
