/*
  vecutil.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Apr-2024
    
  Convenience utilities for vectors
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#ifndef H_vecutil
#define H_vecutil

#include <vector>

//
//  Use it something like this:
//
//  userPtr user = find_in<userPtr>(_server->_users, "fred"",
//    [](userPtr &c) { return c->_nick; });
//

template<typename T>
T find_in(vector<T> &ve, const string &token, string (* member)(T &)) {

  auto i = find_if(ve.begin(), ve.end(), 
    [&token, &member](T &e) { return member(e) == token; });
  if (i == ve.end()) {
    return 0;
  }
  return *i;
  
}

#endif // H_vecutil
