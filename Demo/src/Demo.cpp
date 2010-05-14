#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <string>

namespace fs = boost::filesystem;
namespace io = boost::asio;
using boost::asio::ip::tcp;

void print(const boost::system::error_code& /*e*/,
    io::deadline_timer *t, int *count)
{
  if ( *count < 5 ) {
    std::cout << "Hello, world!\n";
    ++(*count);
    t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
    t->async_wait(boost::bind(print, io::placeholders::error, t, count));
  }
}

void start_timer() {
  io::io_service io;
  int count = 0;
  io::deadline_timer t(io, boost::posix_time::seconds(1));
  t.async_wait(boost::bind(print, io::placeholders::error, &t, &count));
  io.run();
}

class printer {
  io::deadline_timer d_timer0, d_timer1;
  io::strand d_strand;
  int d_count;
public:
  printer(io::io_service& io)
    : d_timer0(io, boost::posix_time::seconds(0)),
      d_timer1(io, boost::posix_time::seconds(0)),
      d_strand(io),
      d_count(0) {
    reset0();
    reset1();
  }
  ~printer() {
   std::cout << "Bye" << std::endl;
  }
  void reset0() {
    d_timer0.expires_at(d_timer0.expires_at() + boost::posix_time::seconds(1));
    d_timer0.async_wait(d_strand.wrap(boost::bind(&printer::print0, this)));
  }
  void reset1() {
    d_timer1.expires_at(d_timer1.expires_at() + boost::posix_time::seconds(2));
    d_timer1.async_wait(d_strand.wrap(boost::bind(&printer::print1, this)));
  }

  void print0() {
    if ( d_count < 10 ) {
      std::cout << "Timer 0\n";
      ++d_count;
      reset0();
    }
  }
  void print1() {
      if ( d_count < 10 ) {
        std::cout << "Timer 1\n";
        ++d_count;
        reset1();
      }
    }
};

int regex_test() {
  std::string line;
  boost::regex pat( "(T.*)" );
  fs::path datafile("src/datafile");
  fs::ifstream bfstr;

  if ( !bfstr ) {
    std::cout << "File not found" << std::endl;
    return -1;
  }

  bfstr.open(datafile, fs::fstream::in);
  while ( !bfstr.eof() ) {
    bfstr >> line;
    boost::smatch matches;
    if ( boost::regex_match(line, matches, pat) )
      std::cout << "> " << matches[0] << std::endl;
  }
  bfstr.close();

  fs::path dir_path(".");
  fs::directory_iterator end_itr;
  for ( fs::directory_iterator itr( dir_path ); itr != end_itr; ++itr ) {
    std::cout << itr->leaf() << std::endl;
  }
  return 0;
}

int threads() {
	io::io_service io;
	printer p(io);
	boost::thread t(boost::bind(&io::io_service::run, &io));
	io.run();
	t.join();
	return 0;
}


int main(int argc, char* argv[]) {

	try {
		if ( argc != 2 ) {
      std::cerr << "Usage: client <host>" << std::endl;
      return 1;
		}
    io::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(argv[1], "8181");
    tcp::resolver::iterator itr = resolver.resolve(query);
    tcp::resolver::iterator end;

    tcp::socket socket(io_service);
    boost::system::error_code error = io::error::host_not_found;
    while ( error && itr != end ) {
    	socket.close();
    	socket.connect(*itr++, error);
    }
    if ( error )
    	throw boost::system::system_error(error);

    for(;;) {
    	boost::array<char, 128> buf;
    	boost::system::error_code error;

    	size_t len = socket.read_some(io::buffer(buf), error);
    	if ( error == io::error::eof )
    		break; // Connection closed by peer
    	else if ( error )
    		throw boost::system::system_error(error);

    	std::cout.write(buf.data(), len);
    }

	} catch ( std::exception &e ) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
