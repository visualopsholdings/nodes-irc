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
#include <boost/thread/thread.hpp>
#include <boost/chrono/duration.hpp>

using namespace std;
using json = nlohmann::json;

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

	boost::shared_ptr<Session> session = Session::create(this, _io_service);

	_acceptor.async_accept(session->socket(),
			boost::bind(&Server::handle_accept, this, session,
					boost::asio::placeholders::error));
}

void Server::handle_accept(boost::shared_ptr<Session> session,
		const boost::system::error_code& error) {

	if (!error) {
		session->start();
		_sessions.push_back(session);

	}
	start_accept();
}

boost::optional<json::iterator> Server::get(json *json, const string &name) {

  json::iterator i = json->find(name);
  if (i == json->end()) {
    return boost::none;
  }
  return i;
  
}


void Server::login(const std::string &username) {
  
	std::string m = "{ \"type\": \"login\", \"username\": \"" + username + "\"}";
	zmq::message_t msg(m.length());
	memcpy(msg.data(), m.c_str(), m.length());
  _req->send(msg);

  // and wait for reply.
  zmq::message_t reply;
  _req->recv(&reply);
  std::string r((const char *)reply.data(), reply.size());
  json doc = json::parse(r);
  boost::optional<json::iterator> type = get(&doc, "type");
  if (!type) {
    cout << "no type" << endl;
    return;
  }
  if (**type == "user") {
    boost::optional<json::iterator> name = get(&doc, "name");
    if (!name) {
      cout << "no name" << endl;
      return;
    }
    boost::optional<json::iterator> id = get(&doc, "id");
    if (!id) {
      cout << "no id" << endl;
      return;
    }
    boost::shared_ptr<Session> session = find_session_username(**name);
    if (!session) {
      cout << "no session" << endl;
      return;
    }
    session->_id = **id;
    session->send("001", { session->_nick, ":Welcome" });
    session->send("002", { session->_nick, ":Your host is localhost running version 1" });
    session->send("004", { session->_nick, "ZMQIRC", "1" });
    session->send("MODE", { session->_nick, "+w" });
//     m.streams.forEach(s => {
//       var channelName = "#" + s.name.replace(/ /g, '+').toLowerCase();
//       server.createChannel(channelName, s.id, s.policy);
//     });
  }
  else {
    cout << "unknown reply type " << **type << endl;
  }
}

boost::shared_ptr<Session> Server::find_session_username(const std::string &username) {
  std::vector<boost::shared_ptr<Session> >::iterator i = find_if(_sessions.begin(), _sessions.end(),
    [&username](boost::shared_ptr<Session> &session) { return session->_username == username; });
  if (i == _sessions.end()) {
    return boost::shared_ptr<Session>();
  }
  return *i;
}
