/*
	server.cpp
	
	Author: Paul Hamilton (paul@visualops.com)
	Date: 16-Apr-2024
		
	Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
	https://github.com/visualopsholdings/zmqirc
*/

#include "server.hpp"
#include "session.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace std;

Server::Server(zmq::socket_t *sub, zmq::socket_t *req) :
		_acceptor(_io_service),
		_sub(sub), _req(req) {
		
	boost::asio::ip::tcp::resolver resolver(_io_service);
	boost::asio::ip::tcp::resolver::query query("127.0.0.1", "6667");
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	_acceptor.open(endpoint.protocol());
	_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	_acceptor.bind(endpoint);
	_acceptor.listen();

	start_accept();

}

void Server::run() {
	_io_service.run();
}

void Server::start_accept() {

	boost::shared_ptr<Session> session = Session::create(
			_io_service);

	_acceptor.async_accept(session->socket(),
			boost::bind(&Server::handle_accept, this, session,
					boost::asio::placeholders::error));
}

void Server::handle_accept(boost::shared_ptr<Session> session,
		const boost::system::error_code& error) {

	if (!error) {
		session->start();

		// TBD: Add the connection

	}
	start_accept();
}
