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

using namespace std;

Session::Session(boost::asio::io_service& io_service) :
	_socket(io_service), _request(this) {
}
	
boost::shared_ptr<Session> Session::create(boost::asio::io_service& io_service) {

	return boost::shared_ptr<Session>(new Session(io_service));

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

	/* Check for error */
	if (!error) {

		_request.handle();

		boost::asio::async_read_until(_socket, _buffer, "\r\n",
				boost::bind(&Session::handle_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));

	} else {

		// TBD: Handle error
	}
}

void Session::nick(const std::list<std::string> &args) {
  _nick = args.front();
}

void Session::user(const std::list<std::string> &args) {
}
