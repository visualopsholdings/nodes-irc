/*
	commands.cpp
	
	Author: Paul Hamilton (paul@visualops.com)
	Date: 16-Apr-2024
		
	Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
	https://github.com/visualopsholdings/zmqirc
*/

#include "session.hpp"

#include "user.hpp"
#include "server.hpp"
#include "zmqclient.hpp"
#include "parser.hpp"
#include "channel.hpp"

#include <boost/log/trivial.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>

void Session::listCmd(const vector<string> &args) {

  if (!_user) {
	  BOOST_LOG_TRIVIAL(error) << "LIST session has no user";
	  return;
	  
  }

  vector<string> names;
  _server->channel_names(&names);

	BOOST_LOG_TRIVIAL(trace) << "got " << names.size() << " names";

  itemsType items;
  items.push_back({ "321", { _user->_nick, "Channel", ":Users", "Name" } });
  for (auto i : names) {
    items.push_back({  "322", { _user->_nick, i, "0" } });
  }
  items.push_back({ "323", {  _user->_nick, ":End of /LIST" } });
  send(_server, items);
  
	BOOST_LOG_TRIVIAL(trace) << "end of list";
}

void Session::joinCmd(const vector<string> &args) {

  if (args.size() < 1) {
	  BOOST_LOG_TRIVIAL(error) << "JOIN missing channel";
	  return;
  }
  if (!_user) {
	  BOOST_LOG_TRIVIAL(error) << "JOIN session has no user";
	  return;
  }
  
	BOOST_LOG_TRIVIAL(debug) << "JOIN channels " << args.front();
  list<string> channels;
  boost::split(channels, args.front(), boost::is_any_of(","));
  for (list<string>::iterator i=channels.begin(); i != channels.end(); i++) {
    string name = Parser::normalise(*i);
    channelPtr chan = _server->find_channel(name);
    if (chan) {
      chan->join(_user);
      // username and nick name are the same.
      itemsType items;
      items.push_back({ "JOIN", { chan->_name, _user->_nick, _user->_realname } });
      items.push_back({ "331", { _user->_nick, chan->_name, ":No topic is set." } });
      send(_server, items);
      // other users
      _server->_zmq->policy_users(chan->_policy);
    }
    else {
	    BOOST_LOG_TRIVIAL(error) << "channel " << name << " not found";
    }
  }

}

void Session::msgCmd(const vector<string> &args) {

  if (args.size() < 2) {
	  BOOST_LOG_TRIVIAL(error) << "MSG missing channel or message";
	  return;
  }
  
  channelPtr chan = _server->find_channel(args.front());
  if (!chan) {
    BOOST_LOG_TRIVIAL(error) << "channel " << args.front() << " not found";
    return;
  }
  
  _server->_zmq->send(_user, chan, args[1]);
}

void Session::capCmd(const vector<string> &args) {

  if (args.size() < 1) {
	  BOOST_LOG_TRIVIAL(error) << "CAP missing caps";
	  return;
  }
  BOOST_LOG_TRIVIAL(info) << "CAP not used";
  
}

void Session::whoCmd(const vector<string> &args) {

  if (args.size() < 1) {
	  BOOST_LOG_TRIVIAL(error) << "WHO missing channel";
	  return;
  }
  BOOST_LOG_TRIVIAL(info) << "WHO not used";
  
}

void Session::modeCmd(const vector<string> &args) {

  if (args.size() < 1) {
	  BOOST_LOG_TRIVIAL(error) << "MODE missing channel";
	  return;
  }
  BOOST_LOG_TRIVIAL(info) << "MODE not used";
  
}

void Session::quitCmd(const vector<string> &args) {

  if (args.size() < 1) {
	  BOOST_LOG_TRIVIAL(error) << "QUIT missing reason";
	  return;
  }
	BOOST_LOG_TRIVIAL(info) << "QUIT because " << args.front();
	if (!_user) {
	  BOOST_LOG_TRIVIAL(warning) << "QUIT ignored because no user yet";
	  return;
	}
  _server->remove_session(shared_from_this());
  
}

void Session::passCmd(const vector<string> &args) {

  if (args.size() < 1) {
	  BOOST_LOG_TRIVIAL(error) << "PASS missing password";
	  return;
  }

  BOOST_LOG_TRIVIAL(info) << "Logging in...";
  
  _server->_zmq->login(_id, args[0]);
  
}
