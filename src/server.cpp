/*
	server.cpp
	
	Author: Paul Hamilton (paul@visualops.com)
	Date: 16-Apr-2024
		
	Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
	https://github.com/visualopsholdings/zmqirc
*/

#include "server.hpp"

#include "tcpsession.hpp"
#include "sslsession.hpp"
#include "channel.hpp"
#include "user.hpp"
#include "parser.hpp"
#include "vecutil.hpp"
#include "zmqclient.hpp"

#include <iostream>
#include <boost/log/trivial.hpp>

Server::Server(const string &version, zmq::socket_t *sub, zmq::socket_t *req, int port, const string &certFile, const string &chainFile) :
    _version(version), _context(boost::asio::ssl::context::method::sslv23), _acceptor(_io_service), _ssl(false) {
		
	_zmq = zmqClientPtr(new ZMQClient(this, sub, req));
  _zmq->run();
	
	boost::asio::ip::tcp::endpoint const endpoint{{}, (boost::asio::ip::port_type)port};
	_acceptor.open(boost::asio::ip::tcp::v4());
	_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	_acceptor.bind(endpoint);
	_acceptor.listen();

  if (!certFile.empty() && !chainFile.empty()) {
    SSLSession::setup(&_context, chainFile, certFile);
    _ssl = true;
  }
  
	start_accept();
}

Server::~Server() {
}

void Server::run() {
	_io_service.run();
}

void Server::start_accept() {

	BOOST_LOG_TRIVIAL(info) << "accepting...";

  sessionPtr session;
  if (_ssl) {
	  BOOST_LOG_TRIVIAL(info) << "using SSL session.";
    session.reset(new SSLSession(this, _io_service, _context));
  }
  else {
    session.reset(new TCPSession(this, _io_service));
  }

	_acceptor.async_accept(session->socket(),
			bind(&Server::handle_accept, this, session,
					boost::asio::placeholders::error));
}

void Server::handle_accept(sessionPtr session,
		const boost::system::error_code& error) {

	if (error) {
	  BOOST_LOG_TRIVIAL(error) << error.message();
		return;
  }
  
	BOOST_LOG_TRIVIAL(info) << "accepted.";

	_sessions.push_back(session);
  stringstream ss;
  ss << _sessions.size();
	session->set_id(ss.str());
	session->start();
	start_accept();
}

void Server::add_user(userPtr user) {

  // thread safe.
//  lock_guard<mutex> guard(_users_mutex);
  
  _users.push_back(user);
}

void Server::remove_session(sessionPtr session) {

  BOOST_LOG_TRIVIAL(debug) << _sessions.size() << " sessions when removing";
  
  vector<sessionPtr>::iterator i = find_if(_sessions.begin(), _sessions.end(), 
    [&session](sessionPtr &e) { return e->_user ? e->_user->_id == session->_user->_id : false; });
  if (i == _sessions.end()) {
    BOOST_LOG_TRIVIAL(error) << "session not found to remove " << session->_user->_id;
    return;
  }
  _sessions.erase(i);
  BOOST_LOG_TRIVIAL(debug) << _sessions.size() << " sessions after removing";
  
}

channelPtr Server::find_channel_policy(const string &policy) {

  // thread safe.
//  lock_guard<mutex> guard(_channels_mutex);
  
  return find_in<channelPtr>(_channels, policy,
      [](channelPtr &c) { return c->_policy; });

}

channelPtr Server::find_channel_(const string &name) {

  return find_in<channelPtr>(_channels, name,
    [](channelPtr &c) { return c->_name; });

}

channelPtr Server::find_channel(const string &name) {

  // thread safe.
//  lock_guard<mutex> guard(_channels_mutex);
  
  return find_channel_(name);

}

channelPtr Server::find_channel_stream(const string &stream) {

  // thread safe.
//  lock_guard<mutex> guard(_channels_mutex);
  
  return find_in<channelPtr>(_channels, stream,
    [](channelPtr &c) { return c->_id; });

}

userPtr Server::find_user_id(const string &id) {

  // thread safe.
//  lock_guard<mutex> guard(_users_mutex);
  
  return find_in<userPtr>(_users, id,
    [](userPtr &c) { return c->_id; });

}

userPtr Server::find_user_nick(const string &nick) {

  // thread safe.
//  lock_guard<mutex> guard(_users_mutex);
  
  return find_in<userPtr>(_users, nick,
    [](userPtr &c) { return c->_nick; });

}

sessionPtr Server::find_session(const string &id) {

  BOOST_LOG_TRIVIAL(debug) << "searching " << _sessions.size() << " sessions";
  return find_in<sessionPtr>(_sessions, id,
    [](sessionPtr &c) { return c->_id; });

}

sessionPtr Server::find_session_for_nick(const string &nick) {

  BOOST_LOG_TRIVIAL(debug) << "searching " << _sessions.size() << " sessions";
  return find_in<sessionPtr>(_sessions, nick,
    [](sessionPtr &c) { return c->_user ? c->_user->_nick : ""; });

}

void Server::channel_names(vector<string> *names) {

  // thread safe.
//  lock_guard<mutex> guard(_channels_mutex);
  
  for (auto i: _channels) {
    names->push_back(i->_name);
  }
  
}

void Server::create_channel(const string &name, const string &id, const string &policy) {

  // thread safe.
//  lock_guard<mutex> guard(_channels_mutex);
  
  string channame = Parser::from_stream_name(name);
  
  BOOST_LOG_TRIVIAL(info) << "create channel " << channame;
  
  channelPtr channel = find_channel_(channame);
  if (channel) {
    BOOST_LOG_TRIVIAL(info) << "channel " << name << " already exists";
    if (channel->_id != id || channel->_policy != policy) {
      BOOST_LOG_TRIVIAL(error) << "channel " << name << " has different id or policy";
    }
    return;
  }
  
  _channels.push_back(channelPtr(new Channel(this, channame, id, policy)));
  
}

// Prefixable
string Server::prefix() {
  return ":localhost";
}

