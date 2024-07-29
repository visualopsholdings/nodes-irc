/*
  user.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Apr-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes-irc
*/

#include "user.hpp"

User::User(const string &id, const string &nick, const string &realname) : 
  _id(id), _nick(nick), _realname(realname) {
}

string User::id() {
  return _id;
}

string User::prefix() {
  // TBD: construct this properly!!!!
  return ":" + _nick + "!" + _nick + "@" + _nick;
}
