/*
	session.cpp
	
	Author: Paul Hamilton (paul@visualops.com)
	Date: 16-Apr-2024
		
	Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
	https://github.com/visualopsholdings/zmqirc
*/

#include "session.hpp"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace std;

Session::Session(Server *server, boost::asio::io_service& io_service) :
	_server(server), _socket(io_service), _request(this) {
}
	
boost::shared_ptr<Session> Session::create(Server *server, boost::asio::io_service& io_service) {

	return boost::shared_ptr<Session>(new Session(server, io_service));

}

boost::asio::ip::tcp::socket& Session::socket() {
	return _socket;
}

boost::asio::streambuf& Session::buffer() {
	return _buffer;
}


void Session::start(void) {

	BOOST_LOG_TRIVIAL(info) << "session started ";
	boost::asio::async_read_until(_socket, _buffer, "\r\n",
			boost::bind(&Session::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

}

void Session::handle_read(const boost::system::error_code& error,
		const std::size_t bytes_transferred) {

	if (error) {
		// TBD: Handle error
		return;
  }
  
  _request.handle();

  boost::asio::async_read_until(_socket, _buffer, "\r\n",
      boost::bind(&Session::handle_read, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));

}

void Session::handle_write(const boost::system::error_code& error) {

	if (error) {
    // TBD: Handle error;
	}
	
}

void Session::write(const std::string &line) {

	boost::asio::async_write(_socket, boost::asio::buffer(line + "\r\n"),
			boost::bind(&Session::handle_write, shared_from_this(),
					boost::asio::placeholders::error));

}

void Session::send(const std::string &cmd, const std::list<std::string> &args) {
  
  write(":localhost " + cmd + " " + boost::algorithm::join(args, " "));
  
}

