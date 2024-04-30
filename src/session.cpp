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
#include "vecutil.hpp"

#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>

Session::Session(Server *server, boost::asio::io_service& io_service) :
	_server(server), _socket(io_service) {

  _commands["NICK"] = bind( &Session::nickCmd, this, placeholders::_1 );
  _commands["USER"] = bind( &Session::userCmd, this, placeholders::_1 );
  _commands["LIST"] = bind( &Session::listCmd, this, placeholders::_1 );
  _commands["JOIN"] = bind( &Session::joinCmd, this, placeholders::_1 );
  _commands["PRIVMSG"] = bind( &Session::msgCmd, this, placeholders::_1 );

}
	
void Session::start(void) {

	BOOST_LOG_TRIVIAL(info) << "session started ";
	boost::asio::async_read_until(_socket, _buffer, "\r\n",
			bind(&Session::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

}

void Session::join(channelPtr channel) {

}

void Session::handle_read(const boost::system::error_code& error,
		const size_t bytes_transferred) {

	if (error) {
	  BOOST_LOG_TRIVIAL(error) << error.message();
		return;
  }
  
//  _request.handle();
  handle_request();

  boost::asio::async_read_until(_socket, _buffer, "\r\n",
      bind(&Session::handle_read, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));

}

void Session::handle_write(const boost::system::error_code& error) {

	if (error) {
	  BOOST_LOG_TRIVIAL(error) << error.message();
	}
	
}

void Session::write(const string &line) {

	boost::asio::async_write(_socket, boost::asio::buffer(line + "\r\n"),
			bind(&Session::handle_write, shared_from_this(),
					boost::asio::placeholders::error));

}

void Session::send(Prefixable *prefix, const string &cmd, const list<string> &args) {
  
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
  vector<string> args;
  string cmd = Parser::parse(line, &args);
  
  // execute the handler.
  map<string, cmdHandler>::iterator handler = _commands.find(cmd);
  if (handler == _commands.end()) {
	  BOOST_LOG_TRIVIAL(warning) << "ignoring " << cmd << " [" << boost::algorithm::join(args, ", ") << "]";
    return;
  }
  handler->second(args);
  
}

void Session::nickCmd(const vector<string> &args) {

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
  userPtr user = find_in<userPtr>(_server->_users, args.front(),
    [](userPtr &c) { return c->_nick; });
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
  _user = userPtr(new User(args.front()));
  _server->_users.push_back(_user);
}

void Session::userCmd(const vector<string> &args) {

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

void Session::listCmd(const vector<string> &args) {

  if (!_user) {
	  BOOST_LOG_TRIVIAL(error) << "LIST session has no user";
	  return;
	  
  }

  send(_server, "321", { _user->_nick, "Channel", ":Users", "Name" });
  for (vector<channelPtr >::iterator i = _server->_channels.begin(); i != _server->_channels.end(); i++) {
    send(_server, "322", { _user->_nick, (*i)->_name, "0" });
  }
  send(_server, "323", {  _user->_nick, ":End of /LIST" });

}

void Session::joinCmd(const vector<string> &args) {

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
    string name = Parser::normalise(*i);
    channelPtr chan = _server->find_channel(name);
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

void Session::msgCmd(const vector<string> &args) {

  if (args.size() < 2) {
	  BOOST_LOG_TRIVIAL(error) << "MSG missing channel or message";
	  return;
  }
  
  channelPtr chan = _server->find_channel(args.front());
  if (!chan) {
    BOOST_LOG_TRIVIAL(error) << "channel " << args.front() << " not found";
    return;
  }
  
  _server->send(_user, chan, args[1].substr(1));
}
