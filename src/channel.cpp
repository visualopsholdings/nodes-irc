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

#include <boost/algorithm/string.hpp>
#include <regex>
#include <boost/log/trivial.hpp>

using namespace std;

Channel::Channel(Server *server, const string &name, const string &id, const string &policy) :
	_server(server), _name(name), _id(id), _policy(policy) {
}
	
boost::shared_ptr<Channel> Channel::create(Server *server, const string &name, const string &id, const string &policy) {

	return boost::shared_ptr<Channel>(new Channel(server, name, id, policy));

}

void Channel::join(boost::shared_ptr<User> user) {

  if (find(_users.begin(), _users.end(), user) != _users.end()) {
    BOOST_LOG_TRIVIAL(warning) << "user already joined";
    return;
  }
  _users.push_back(user);
  
}

boost::shared_ptr<User> Channel::find_user_id(const string &id) {

  vector<boost::shared_ptr<User> >::iterator i = find_if(_users.begin(), _users.end(),
    [&id](boost::shared_ptr<User> &user) { return user->_id == id; });
  if (i == _users.end()) {
    return boost::shared_ptr<User>();
  }
  return *i;

}

void Channel::send(Prefixable *prefix, const std::string &cmd, const std::list<std::string> &args) {

  for (vector<boost::shared_ptr<User> >::iterator i=_users.begin(); i != _users.end(); i++) {
    boost::shared_ptr<Session> session = _server->find_session_for_nick((*i)->_nick);
    if (session) {
      session->send(prefix, cmd, args);
    }
  }
}

string Channel::normalise(const string &chan) {

  string s = chan;
  boost::trim(s);
  boost::to_lower(s);
  // TBD: {, } and | are uppercase variants of [, ] and \ respectively
  return s;
}

string Channel::from_stream_name(const string &stream) {

  string name = "#" + stream;
  regex r(" ");
  return normalise(regex_replace(name, r, "+"));

}
