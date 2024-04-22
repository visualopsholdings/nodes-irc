/*
  parser.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 18-Apr-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include "parser.hpp"

#include <boost/algorithm/string.hpp>

using namespace std;

string Parser::parse(const string &line, list<string> *args) {

  boost::split(*args, line, boost::is_any_of(" "));
  string cmd = args->front();
  args->pop_front();
  return cmd;
  
}