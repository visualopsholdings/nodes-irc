/*
  prefixable.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 29-Apr-2024
    
  Abstract interface for things that generate a prefix.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#ifndef H_prefixable
#define H_prefixable

#include <string>

class Prefixable {

public:

  virtual const std::string prefix() = 0;
  
};

#endif // H_prefixable
