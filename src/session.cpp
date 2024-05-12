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
#include "zmqclient.hpp"

#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string.hpp>

Session::Session(Server *server, boost::asio::io_service& io_service) :
	_server(server) {

  _commands["CAP"] = bind( &Session::capCmd, this, placeholders::_1 );
  _commands["PASS"] = bind( &Session::passCmd, this, placeholders::_1 );
  _commands["LIST"] = bind( &Session::listCmd, this, placeholders::_1 );
  _commands["JOIN"] = bind( &Session::joinCmd, this, placeholders::_1 );
  _commands["PRIVMSG"] = bind( &Session::msgCmd, this, placeholders::_1 );
  _commands["WHO"] = bind( &Session::whoCmd, this, placeholders::_1 );
  _commands["MODE"] = bind( &Session::modeCmd, this, placeholders::_1 );
  _commands["QUIT"] = bind( &Session::quitCmd, this, placeholders::_1 );

}
	
void Session::set_id(const string &id) {

  _id = id;
    
}

void Session::start() {

	BOOST_LOG_TRIVIAL(info) << "session started ";
	read();

}

void Session::handle_read(const boost::system::error_code& error,
		const size_t bytes_transferred) {

	if (error) {
	  BOOST_LOG_TRIVIAL(error) << error.message();
		return;
  }
  
  handle_request();
	read();

}

void Session::handle_write(const boost::system::error_code& error) {

	if (error) {
	  BOOST_LOG_TRIVIAL(error) << error.message();
	}
	
}

void Session::send(Prefixable *prefix, const string &cmd, const list<string> &args) {
  
  // thread safe.
  lock_guard<mutex> guard(_socket_mutex);
  
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
  
  BOOST_LOG_TRIVIAL(trace) << line;

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

void Session::set_user_details(const string &id, const string &name, const string &fullname) {

  // thread safe.
  lock_guard<mutex> guard(_user_mutex);
  
  if (_user) {
    BOOST_LOG_TRIVIAL(error) << "User already exists.";
	  return;
  }

  _user = userPtr(new User(id, name, ":" + fullname));
  _server->add_user(_user);
}

void Session::send_banner() {

  // thread safe.
  lock_guard<mutex> guard(_user_mutex);
  
  send(_server, "001", { _user->_nick, ":Welcome" });
  send(_server, "002", { _user->_nick, ":Your host is localhost running version 1" });
  send(_server, "004", { _user->_nick, _server->_version });
  send(_server, "MODE", { _user->_nick, "+w" });
    
}

