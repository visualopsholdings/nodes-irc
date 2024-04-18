/*
  request.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 18-Apr-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include "request.hpp"

#include "session.hpp"

#include <iostream>
#include <sstream>

using namespace std;

Request::Request(Session* session) :
  _session(session) {
}

void Request::handle() {

  std::string line;
  std::istream is(&_session->buffer());
  std::getline(is, line, '\n');

  // Erase \r at the end
  line.erase(line.size() - 1, 1);
  
  cout << "GOT: " << line << endl;

}
