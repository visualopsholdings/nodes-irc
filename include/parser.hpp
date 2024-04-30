/*
  parser.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Apr-2024
    
  A parser for IRC commands.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#ifndef H_parser
#define H_parser

#include <string>
#include <list>

using namespace std;

class Parser {

public:

  static string parse(const string &line, vector<string> *args);
    // parse the line, returning the CMD and list of possible arguments.
  
  static string from_stream_name(const string &stream);
    // create a channel name from a stream name.
    
  static string normalise(const string &chan);
    // normalse a channel name.

};

#endif // H_parser
