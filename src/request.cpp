/*
  request.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 18-Apr-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include "request.hpp"

#include "session.hpp"
#include "parser.hpp"
#include "server.hpp"

#include <iostream>
#include <sstream>
#include <boost/algorithm/string/join.hpp>
#include <boost/log/trivial.hpp>

using namespace std;

Request::Request(Session* session) :
  _session(session) {

	_commands["NICK"] = Request::nick;
	_commands["USER"] = Request::user;
	_commands["LIST"] = Request::listCmd;
	_commands["JOIN"] = Request::join;
	
}

void Request::handle() {

  string line;
  istream is(&_session->_buffer);
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
  handler->second(_session, args);
  
}


void Request::nick(Session *session, const list<string> &args) {

  session->_nick = args.front();
  
}

void Request::user(Session *session, const list<string> &args) {

  session->_username = args.front();
  session->_server->login(session->_username);

}

void Request::listCmd(Session *session, const list<string> &args) {

// :localhost 321 tracy Channel :Users  Name
// :localhost 322 tracy #my+conversation 0 
// :localhost 322 tracy #welcome 0 
// :localhost 323 tracy :End of /LIST

}

void Request::join(Session *session, const list<string> &args) {

}