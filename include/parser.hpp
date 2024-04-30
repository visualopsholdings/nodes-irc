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

class Parser {

public:

  static std::string parse(const std::string &line, std::vector<std::string> *args);
    // parse the line, returning the CMD and list of possible arguments.
  
};

#endif // H_parser
