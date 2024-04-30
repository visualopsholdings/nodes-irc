/*
	server.cpp
	
	Author: Paul Hamilton (paul@visualops.com)
	Date: 16-Apr-2024
		
	Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
	https://github.com/visualopsholdings/zmqirc
*/

#include "server.hpp"

#include "session.hpp"
#include "channel.hpp"
#include "user.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/log/trivial.hpp>

Server::Server(zmq::socket_t *sub, zmq::socket_t *req) :
		_acceptor(_io_service),
		_sub(sub), _req(req) {
		
	boost::asio::ip::tcp::resolver resolver(_io_service);
	boost::asio::ip::tcp::resolver::query query("127.0.0.1", "6667");
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	_acceptor.open(endpoint.protocol());
	_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	_acceptor.bind(endpoint);
	_acceptor.listen();

	start_accept();

}

Server::~Server() {
}

void Server::run() {
	_io_service.run();
}

void Server::start_accept() {

	sessionPtr session = Session::create(this, _io_service);

	_acceptor.async_accept(session->_socket,
			bind(&Server::handle_accept, this, session,
					boost::asio::placeholders::error));
}

void Server::handle_accept(sessionPtr session,
		const boost::system::error_code& error) {

	if (!error) {
		session->start();
		_sessions.push_back(session);

	}
	start_accept();
}

optional<json::iterator> Server::get(json *json, const string &name) {

  json::iterator i = json->find(name);
  if (i == json->end()) {
    return nullopt;
  }
  return i;
  
}

void Server::login(const string &username) {
  
	string m = "{ \"type\": \"login\", \"username\": \"" + username + "\"}";
	zmq::message_t msg(m.length());
	memcpy(msg.data(), m.c_str(), m.length());
  _req->send(msg);

  // and wait for reply.
  zmq::message_t reply;
  _req->recv(&reply);
  string r((const char *)reply.data(), reply.size());
  json doc = json::parse(r);
  optional<json::iterator> type = get(&doc, "type");
  if (!type) {
	  BOOST_LOG_TRIVIAL(error) << "no type";
    return;
  }
  if (**type == "user") {
    optional<json::iterator> name = get(&doc, "name");
    if (!name) {
	    BOOST_LOG_TRIVIAL(error) << "no name for user";
      return;
    }
    optional<json::iterator> id = get(&doc, "id");
    if (!id) {
	    BOOST_LOG_TRIVIAL(error) << "no id for user";
      return;
    }
    sessionPtr session = find_session_username(**name);
    if (!session) {
	    BOOST_LOG_TRIVIAL(error) << "no session for " << **name;
      return;
    }
	  BOOST_LOG_TRIVIAL(info) << "session assigned to " << session->_user->_nick;
    session->_user->_id = **id;
    session->send(this, "001", { session->_user->_nick, ":Welcome" });
    session->send(this, "002", { session->_user->_nick, ":Your host is localhost running version 1" });
    session->send(this, "004", { session->_user->_nick, "ZMQIRC", "1" });
    session->send(this, "MODE", { session->_user->_nick, "+w" });
    
    optional<json::iterator> streams = get(&doc, "streams");
    if (streams) {
      if ((**streams).is_array()) {
        for (json::iterator i = (**streams).begin(); i != (**streams).end(); i++) {
          optional<json::iterator> id = get(&(*i), "id");
          optional<json::iterator> name = get(&(*i), "name");
          optional<json::iterator> policy = get(&(*i), "policy");
          if (id && name && policy) {
            create_channel(**name, **id, **policy);
          }
          else {
	          BOOST_LOG_TRIVIAL(error) << "stream not complete";
          }
        }
      }
      else {
	      BOOST_LOG_TRIVIAL(error) << "streams not array";
      }
    }
    else {
	      BOOST_LOG_TRIVIAL(error) << "missing streams";
    }
  }
  else {
	  BOOST_LOG_TRIVIAL(error) << "unknown reply type " << **type;
  }
}

void Server::policy_users(const string &policy) {

	string m = "{ \"type\": \"policyusers\", \"policy\": \"" + policy + "\"}";
	zmq::message_t msg(m.length());
	memcpy(msg.data(), m.c_str(), m.length());
  _req->send(msg);

  // and wait for reply.
  zmq::message_t reply;
  _req->recv(&reply);
  string r((const char *)reply.data(), reply.size());
  json doc = json::parse(r);
  optional<json::iterator> type = get(&doc, "type");
  if (!type) {
	  BOOST_LOG_TRIVIAL(error) << "no type";
    return;
  }
  if (**type == "policyusers") {
    optional<json::iterator> id = get(&doc, "id");
    if (!id) {
	    BOOST_LOG_TRIVIAL(error) << "no id for policyusers";
      return;
    }
    channelPtr channel = find_channel_policy(**id);
    if (!channel) {
	    BOOST_LOG_TRIVIAL(error) << "no channel for policy " << **id;
      return;
    }
    optional<json::iterator> users = get(&doc, "users");
    if (users) {
      if ((**users).is_array()) {
        for (json::iterator i = (**users).begin(); i != (**users).end(); i++) {
          optional<json::iterator> id = get(&(*i), "id");
          optional<json::iterator> name = get(&(*i), "name");
          optional<json::iterator> fullname = get(&(*i), "fullname");
          if (id && name && fullname) {
            userPtr user = channel->find_user_id(**id);
            if (!user) {
              user = User::create(**name);
              user->_realname = **fullname;
              user->_id = **id;
              add_user(user);
              channel->join(user);
              channel->send(user.get(), "JOIN", { channel->_name, user->_username, user->_realname });
            }
          }
          else {
	          BOOST_LOG_TRIVIAL(error) << "stream not complete";
          }
        }
      }
      else {
	      BOOST_LOG_TRIVIAL(error) << "users not array";
      }
    }
    else {
	      BOOST_LOG_TRIVIAL(error) << "missing users";
    }
  }
  else {
	  BOOST_LOG_TRIVIAL(error) << "unknown reply type " << **type;
  }

}

void Server::send(userPtr user, channelPtr channel, const string &text) {

	string m = "{ \"type\": \"message\", \"user\": \"" + user->_id + "\", \"stream\": \"" + channel->_id + "\", \"policy\": \"" + channel->_policy + "\", \"text\": \"" + text + "\"}";
	zmq::message_t msg(m.length());
	memcpy(msg.data(), m.c_str(), m.length());
  _req->send(msg);

  // and wait for reply.
  zmq::message_t reply;
  _req->recv(&reply);
  string r((const char *)reply.data(), reply.size());
  json doc = json::parse(r);
  optional<json::iterator> type = get(&doc, "type");
  if (!type) {
	  BOOST_LOG_TRIVIAL(error) << "no type";
    return;
  }
  if (**type == "message") {
    optional<json::iterator> stream = get(&doc, "stream");
    optional<json::iterator> userid = get(&doc, "user");
    optional<json::iterator> text = get(&doc, "text");
    if (stream && userid && text) {
      channelPtr channel = find_channel_stream(**stream);
      if (channel) {
        userPtr user = find_user_id(**userid);
        if (user) {
          channel->send(user.get(), "PRIVMSG", { channel->_name, **text });
        }
        else {
          BOOST_LOG_TRIVIAL(error) << "user not found";
        }
      }
      else {
        BOOST_LOG_TRIVIAL(error) << "channel not found";
      }
    }
    else {
      BOOST_LOG_TRIVIAL(error) << "message not complete";
    }
  }
  else {
	  BOOST_LOG_TRIVIAL(error) << "unknown reply type " << **type;
  }
  
}

channelPtr Server::find_channel_policy(const string &policy) {

  vector<channelPtr >::iterator i = find_if(_channels.begin(), _channels.end(),
    [&policy](channelPtr &channel) { return channel->_policy == policy; });
  if (i == _channels.end()) {
    return 0;
  }
  return *i;

}

channelPtr Server::find_channel_stream(const string &stream) {

  vector<channelPtr >::iterator i = find_if(_channels.begin(), _channels.end(),
    [&stream](channelPtr &channel) { return channel->_id == stream; });
  if (i == _channels.end()) {
    return 0;
  }
  return *i;

}

channelPtr Server::find_channel(const string &name) {

  vector<channelPtr >::iterator i = find_if(_channels.begin(), _channels.end(),
    [&name](channelPtr &channel) { return channel->_name == name; });
  if (i == _channels.end()) {
    return 0;
  }
  return *i;

}

sessionPtr Server::find_session_username(const string &username) {

  vector<sessionPtr >::iterator i = find_if(_sessions.begin(), _sessions.end(),
    [&username](sessionPtr &session) { return session->_user->_username == username; });
  if (i == _sessions.end()) {
    return 0;
  }
  return *i;
  
}

userPtr Server::find_user_id(const string &id) {

  vector<userPtr >::iterator i = find_if(_users.begin(), _users.end(),
    [&id](userPtr &user) { return user->_id == id; });
  if (i == _users.end()) {
    return 0;
  }
  return *i;

}

userPtr Server::find_user_nick(const string &nick) {

  vector<userPtr >::iterator i = find_if(_users.begin(), _users.end(),
    [nick](userPtr &user) { return user->_nick == nick; });
  if (i == _users.end()) {
    return 0;
  }
  return *i;

}

void Server::add_user(userPtr user) {
  _users.push_back(user);
}

sessionPtr Server::find_session_for_nick(const string &nick) {

  vector<sessionPtr >::iterator i = find_if(_sessions.begin(), _sessions.end(),
    [&nick](sessionPtr &session) { return session->_user->_nick == nick; });
  if (i == _sessions.end()) {
    return 0;
  }
  return *i;

}

vector<channelPtr >::iterator Server::end_channel() {
  return _channels.end();
}

vector<channelPtr >::iterator Server::begin_channel() {
  return _channels.begin();
}

void Server::create_channel(const string &name, const string &id, const string &policy) {

  string channame = Channel::from_stream_name(name);
  
  BOOST_LOG_TRIVIAL(info) << "create channel " << channame;
  
  channelPtr channel = find_channel(channame);
  if (channel) {
    BOOST_LOG_TRIVIAL(error) << "channel already exists";
    return;
  }
  channel = Channel::create(this, channame, id, policy);
  _channels.push_back(channel);
  
}

// Prefixable
const string Server::prefix() {
  return ":localhost";
}

