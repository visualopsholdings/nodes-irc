/*
  user.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Apr-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include "user.hpp"

using namespace std;

User::User(const string &nick) :
	_nick(nick) {
}

User::~User() {
}

shared_ptr<User> User::create(const string &nick) {

	return shared_ptr<User>(new User(nick));

}

const string User::prefix() {
  // TBD: construct this properly!!!!
  return ":" + _nick + "!" + _username + "@" + _nick;
}
