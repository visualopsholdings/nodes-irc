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

#include <iostream>
#include <sstream>
#include <boost/algorithm/string/join.hpp>

using namespace std;

Request::Request(Session* session) :
  _session(session) {
}

void Request::handle() {

  string line;
  istream is(&_session->buffer());
  getline(is, line, '\n');

  // Erase \r at the end
  line.erase(line.size() - 1, 1);
  
  list<string> args;
  string cmd = Parser::parse(line, &args);
  if (cmd == "NICK") {
    _session->nick(args);
  }
  else if (cmd == "USER") {
    _session->user(args);
  }
  else {
    cout << "ignoring " << cmd << " [" << boost::algorithm::join(args, ", ") << "]" << endl;
  }
  
}
