/*
  channel.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Apr-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include "channel.hpp"

#include "session.hpp"
#include "user.hpp"
#include "server.hpp"
#include "vecutil.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>

Channel::Channel(Server *server, const string &name, const string &id, const string &policy) :
	_server(server), _name(name), _id(id), _policy(policy) {
}
	
string Channel::id() {
  return _id;
}

string Channel::policy() {
  return _policy;
}

void Channel::join(userPtr user) {

  // thread safe.
  lock_guard<mutex> guard(_users_mutex);
  
  if (find(_users.begin(), _users.end(), user) != _users.end()) {
    BOOST_LOG_TRIVIAL(warning) << "user already joined";
    return;
  }
  _users.push_back(user);
  
  // users username is the same as the nickname.
  send(user.get(), "JOIN", { _name, user->_nick, user->_realname });
  
}

void Channel::send_message(userPtr user, const string &text) {
  
  // thread safe.
  lock_guard<mutex> guard(_users_mutex);
  
  send(user.get(), "PRIVMSG", { _name, ":" + text });
  
}

userPtr Channel::find_user_id(const string &id) {

  return find_in<userPtr>(_users, id,
    [](userPtr &c) { return c->_id; });

}

void Channel::send(Prefixable *prefix, const string &cmd, const list<string> &args) {

  itemsType items;
  items.push_back({ cmd, args });
  
  for (auto i: _users) {
    sessionPtr session = _server->find_session_for_nick(i->_nick);
    if (session) {
      session->send(prefix, items);
    }
  }
}
