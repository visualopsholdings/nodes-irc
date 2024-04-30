/*
	session.cpp
	
	Author: Paul Hamilton (paul@visualops.com)
	Date: 16-Apr-2024
		
	Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
	https://github.com/visualopsholdings/zmqirc
*/

#include "session.hpp"

#include "parser.hpp"
#include "server.hpp"
#include "channel.hpp"
#include "user.hpp"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

Session::Session(Server *server, boost::asio::io_service& io_service) :
	_server(server), _socket(io_service)/*, _request(this)*/ {

  _commands["NICK"] = boost::bind( &Session::nickCmd, this, _1 );
  _commands["USER"] = boost::bind( &Session::userCmd, this, _1 );
  _commands["LIST"] = boost::bind( &Session::listCmd, this, _1 );
  _commands["JOIN"] = boost::bind( &Session::joinCmd, this, _1 );

}
	
boost::shared_ptr<Session> Session::create(Server *server, boost::asio::io_service& io_service) {

	return boost::shared_ptr<Session>(new Session(server, io_service));

}

void Session::start(void) {

	BOOST_LOG_TRIVIAL(info) << "session started ";
	boost::asio::async_read_until(_socket, _buffer, "\r\n",
			boost::bind(&Session::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

}

void Session::join(boost::shared_ptr<Channel> channel) {

}

void Session::handle_read(const boost::system::error_code& error,
		const std::size_t bytes_transferred) {

	if (error) {
	  BOOST_LOG_TRIVIAL(error) << error.message();
		return;
  }
  
//  _request.handle();
  handle_request();

  boost::asio::async_read_until(_socket, _buffer, "\r\n",
      boost::bind(&Session::handle_read, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));

}

void Session::handle_write(const boost::system::error_code& error) {

	if (error) {
	  BOOST_LOG_TRIVIAL(error) << error.message();
	}
	
}

void Session::write(const std::string &line) {

	boost::asio::async_write(_socket, boost::asio::buffer(line + "\r\n"),
			boost::bind(&Session::handle_write, shared_from_this(),
					boost::asio::placeholders::error));

}

void Session::send(Prefixable *prefix, const std::string &cmd, const std::list<std::string> &args) {
  
  write(prefix->prefix() + " " + cmd + " " + boost::algorithm::join(args, " "));
  
}

void Session::handle_request() {

  string line;
  istream is(&_buffer);
  getline(is, line, '\n');

  if (line.size() == 0) {
    return;
  }
  
  // Erase \r at the end
  line.erase(line.size()-1, 1);
  
  // parse the command
  list<string> args;
  string cmd = Parser::parse(line, &args);
  
  // execute the handler.
  map<string, cmdHandler>::iterator handler = _commands.find(cmd);
  if (handler == _commands.end()) {
	  BOOST_LOG_TRIVIAL(warning) << "ignoring " << cmd << " [" << boost::algorithm::join(args, ", ") << "]";
    return;
  }
  handler->second(args);
  
}

void Session::nickCmd(const list<string> &args) {

  if (args.size() < 1) {
	  BOOST_LOG_TRIVIAL(error) << "NICK missing nickname";
	  return;
  }
  if (_user) {
	  if (_user->_nick != args.front()) {
	    BOOST_LOG_TRIVIAL(error) << "NICK session already has user with different nickname";
	  }
	  return;
  }
  // this session has no user yet.
  boost::shared_ptr<User> user = _server->find_user_nick(args.front());
  if (user) {
    if (_server->find_session_for_nick(user->_nick)) {
	    BOOST_LOG_TRIVIAL(error) << "NICK server already has different session for this user";
	    return;
    }
    // this is the session for that user.
    _user = user;
    return;
  }
  
  // create a new user and add it to the server.
  _user = User::create(args.front());
  _server->add_user(_user);
}

void Session::userCmd(const list<string> &args) {

  if (args.size() < 1) {
	  BOOST_LOG_TRIVIAL(error) << "USER missing username";
	  return;
  }
  if (!_user) {
	  BOOST_LOG_TRIVIAL(error) << "USER session has no user";
	  return;
	  
  }
  
  _user->_username = args.front();
  _user->_realname = args.back();
  _server->login(_user->_username);

}

void Session::listCmd(const list<string> &args) {

  if (!_user) {
	  BOOST_LOG_TRIVIAL(error) << "LIST session has no user";
	  return;
	  
  }

  send(_server, "321", { _user->_nick, "Channel", ":Users", "Name" });
  for (std::vector<boost::shared_ptr<Channel> >::iterator i = _server->begin_channel(); i != _server->end_channel(); i++) {
    send(_server, "322", { _user->_nick, (*i)->_name, "0" });
  }
  send(_server, "323", {  _user->_nick, ":End of /LIST" });

}

void Session::joinCmd(const list<string> &args) {

  if (args.size() < 1) {
	  BOOST_LOG_TRIVIAL(error) << "JOIN missing channel";
	  return;
  }
  if (!_user) {
	  BOOST_LOG_TRIVIAL(error) << "JOIN session has no user";
	  return;
	  
  }
  
	BOOST_LOG_TRIVIAL(debug) << "JOIN channels " << args.front();
  list<string> channels;
  boost::split(channels, args.front(), boost::is_any_of(","));
  for (list<string>::iterator i=channels.begin(); i != channels.end(); i++) {
    string name = Channel::normalise(*i);
    boost::shared_ptr<Channel> chan = _server->find_channel(name);
    if (chan) {
      chan->join(_user);
      send(_user.get(), "JOIN", { chan->_name, _user->_username, _user->_realname });
      // other users
      _server->policy_users(chan->_policy);
    }
    else {
	    BOOST_LOG_TRIVIAL(error) << "channel " << name << " not found";
    }
  }

}
