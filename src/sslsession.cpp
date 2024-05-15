/*
	sslsession.cpp
	
	Author: Paul Hamilton (paul@visualops.com)
	Date: 10-May-2024
		
	Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
	https://github.com/visualopsholdings/zmqirc
*/

#include "sslsession.hpp"

#include <boost/log/trivial.hpp>
#include <ostream>

SSLSession::SSLSession(Server *server, boost::asio::io_service& io_service, boost::asio::ssl::context& context) :
	Session(server, io_service), _socket(io_service, context) {

}

boost::asio::basic_socket<boost::asio::ip::tcp, boost::asio::any_io_executor> &SSLSession::socket() {
  return _socket.lowest_layer();
}

void SSLSession::setup(boost::asio::ssl::context *context, const string &chain, const string &cert) {

  context->set_options(
    boost::asio::ssl::context::default_workarounds
    | boost::asio::ssl::context::no_sslv2
  );
  context->use_certificate_chain_file(chain);
  context->use_private_key_file(cert, boost::asio::ssl::context::pem);

}

void SSLSession::start() {

	BOOST_LOG_TRIVIAL(info) << "shaking hands...";
	
  _socket.async_handshake(boost::asio::ssl::stream_base::server,
    bind(&SSLSession::handle_handshake, this,
      boost::asio::placeholders::error));

}

void SSLSession::handle_handshake(const boost::system::error_code& error) {

  if (error) {
	  BOOST_LOG_TRIVIAL(error) << error.message();
		return;
  }

	BOOST_LOG_TRIVIAL(info) << "handshake done.";
  Session::start();
  
}

void SSLSession::read() {

	boost::asio::async_read_until(_socket, _buffer, "\r\n",
			bind(&SSLSession::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

}

void SSLSession::write(const string &line) {

	BOOST_LOG_TRIVIAL(trace) << "writing " << line;

	boost::asio::async_write(_socket, boost::asio::buffer(line + "\r\n\r\n"),
			bind(&SSLSession::handle_write, shared_from_this(),
					boost::asio::placeholders::error));

}
