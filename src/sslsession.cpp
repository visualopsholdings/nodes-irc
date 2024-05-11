/*
	sslsession.cpp
	
	Author: Paul Hamilton (paul@visualops.com)
	Date: 10-May-2024
		
	Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
	https://github.com/visualopsholdings/zmqirc
*/

#include "sslsession.hpp"

SSLSession::SSLSession(Server *server, boost::asio::io_service& io_service, boost::asio::ssl::context& context) :
	Session(server, io_service), _socket(io_service, context) {

}

boost::asio::basic_socket<boost::asio::ip::tcp, boost::asio::any_io_executor> &SSLSession::socket() {
  return _socket.lowest_layer();
}

void SSLSession::read() {

	boost::asio::async_read_until(_socket, _buffer, "\r\n",
			bind(&SSLSession::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

}

void SSLSession::write(const string &line) {

	boost::asio::async_write(_socket, boost::asio::buffer(line + "\r\n"),
			bind(&SSLSession::handle_write, shared_from_this(),
					boost::asio::placeholders::error));

}
