/*
  parser.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 18-Apr-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include "parser.hpp"

#include <boost/algorithm/string.hpp>
#include <regex>

string Parser::parse(const string &line, vector<string> *args) {

  boost::split(*args, line, boost::is_any_of(" "));
  string cmd = args->front();
  args->erase(args->begin());
  return cmd;
  
}

string Parser::normalise(const string &chan) {

  string s = chan;
  boost::trim(s);
  boost::to_lower(s);
  // TBD: {, } and | are uppercase variants of [, ] and \ respectively
  return s;
}

string Parser::from_stream_name(const string &stream) {

  string name = "#" + stream;
  regex r(" ");
  return normalise(regex_replace(name, r, "+"));

}
