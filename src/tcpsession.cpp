/*
	tcpsession.cpp
	
	Author: Paul Hamilton (paul@visualops.com)
	Date: 11-May-2024
		
	Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
	https://github.com/visualopsholdings/zmqirc
*/

#include "tcpsession.hpp"

TCPSession::TCPSession(Server *server, boost::asio::io_service& io_service) :
	Session(server, io_service), _socket(io_service) {

}

boost::asio::basic_socket<boost::asio::ip::tcp, boost::asio::any_io_executor> &TCPSession::socket() {
  return _socket;
}

void TCPSession::read() {

	boost::asio::async_read_until(_socket, _buffer, "\r\n",
			bind(&TCPSession::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

}

void TCPSession::write(const string &line) {

	boost::asio::async_write(_socket, boost::asio::buffer(line + "\r\n"),
			bind(&TCPSession::handle_write, shared_from_this(),
					boost::asio::placeholders::error));

}
