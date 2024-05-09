/*
  zmqclient.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 1-May-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include "zmqclient.hpp"

#include "session.hpp"
#include "user.hpp"
#include "vecutil.hpp"
#include "server.hpp"
#include "channel.hpp"

#include <boost/log/trivial.hpp>
#include <thread>

ZMQClient::ZMQClient(Server *server, zmq::socket_t *sub, zmq::socket_t *req) : 
  _server(server), _sub(sub), _req(req) {

  // expect these as replies
  _reqmessages["user"] = bind( &ZMQClient::userMsg, this, placeholders::_1 );
  _reqmessages["policyusers"] = bind( &ZMQClient::policyUsersMsg, this, placeholders::_1 );
  _reqmessages["message"] = bind( &ZMQClient::messageMsg, this, placeholders::_1 );
  _reqmessages["ack"] = bind( &ZMQClient::ackMsg, this, placeholders::_1 );
  _reqmessages["err"] = bind( &ZMQClient::errMsg, this, placeholders::_1 );
  
  // expect these being published
  _submessages["message"] = bind( &ZMQClient::messageMsg, this, placeholders::_1 );

}

void ZMQClient::run() {

  thread t(bind(&ZMQClient::receive, this));
  t.detach();
  
}

void ZMQClient::receive() {

    zmq::pollitem_t items [] = {
        { *_req, 0, ZMQ_POLLIN, 0 },
        { *_sub, 0, ZMQ_POLLIN, 0 }
    };
    const std::chrono::milliseconds timeout{500};
    while (1) {
    
//      BOOST_LOG_TRIVIAL(debug) << "polling for messages";
      zmq::message_t message;
      zmq::poll(&items[0], 2, timeout);
    
      if (items[0].revents & ZMQ_POLLIN) {
        BOOST_LOG_TRIVIAL(debug) << "got _req message";
        zmq::message_t reply;
        try {
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
          _req->recv(&reply);
#else
          auto res = _req->recv(reply, zmq::recv_flags::none);
#endif
          handle_reply(reply, &_reqmessages);
        }
        catch (zmq::error_t &e) {
          BOOST_LOG_TRIVIAL(warning) << "got exc with _req recv" << e.what() << "(" << e.num() << ")";
        }
      }
      if (items[1].revents & ZMQ_POLLIN) {
        BOOST_LOG_TRIVIAL(debug) << "got _sub message";
        zmq::message_t reply;
        try {
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
          _sub->recv(&reply);
#else
          auto res = _sub->recv(reply, zmq::recv_flags::none);
#endif
          handle_reply(reply, &_submessages);
        }
        catch (zmq::error_t &e) {
          BOOST_LOG_TRIVIAL(warning) << "got exc with _sub recv " << e.what() << "(" << e.num() << ")";
        }
      }
    }

}

void ZMQClient::handle_reply(const zmq::message_t &reply, map<string, msgHandler> *handlers) {

  // convert to JSON
  string r((const char *)reply.data(), reply.size());
  json doc = json::parse(r);

  BOOST_LOG_TRIVIAL(debug) << "got reply " << doc;

  // switch the handler based on the message type.
  optional<json::iterator> type = get(&doc, "type");
  if (!type) {
    BOOST_LOG_TRIVIAL(error) << "no type";
    return;
  }
  map<string, msgHandler>::iterator handler = handlers->find(**type);
  if (handler == handlers->end()) {
    BOOST_LOG_TRIVIAL(error) << "unknown reply type " << **type;
    return;
  }
  handler->second(&doc);
  
}

bool ZMQClient::trySend(const string &m) {

	zmq::message_t msg(m.length());
	memcpy(msg.data(), m.c_str(), m.length());
  try {
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
    _req->send(msg);
#else
    _req->send(msg, zmq::send_flags::none);
#endif
    return true;
  }
  catch (...) {
   return false;
  }

}

void ZMQClient::send(const string &m) {

  if (!trySend(m)) {
    for (int i=0; i<4; i++) {
	    this_thread::sleep_for(chrono::milliseconds(20));
      BOOST_LOG_TRIVIAL(warning) << "retrying send";
	    if (trySend(m)) {
	      return;
	    }
    }
  }

}

void ZMQClient::login(const string &username) {
  
	send("{ \"type\": \"login\", \"username\": \"" + username + "\"}");

}

void ZMQClient::policy_users(const string &policy) {

	send("{ \"type\": \"policyusers\", \"policy\": \"" + policy + "\"}");

}

void ZMQClient::send(userPtr user, channelPtr channel, const string &text) {

	send("{ \"type\": \"message\", \"user\": \"" + user->id() + "\", \"stream\": \"" + channel->id() + "\", \"policy\": \"" + channel->policy() + "\", \"text\": \"" + text + "\"}");

}

optional<json::iterator> ZMQClient::get(json *json, const string &name) {

  json::iterator i = json->find(name);
  if (i == json->end()) {
    return nullopt;
  }
  return i;
  
}

void ZMQClient::userMsg(json *doc) {

  optional<json::iterator> name = get(doc, "name");
  if (!name) {
    BOOST_LOG_TRIVIAL(error) << "no name for user";
    return;
  }
  optional<json::iterator> id = get(doc, "id");
  if (!id) {
    BOOST_LOG_TRIVIAL(error) << "no id for user";
    return;
  }
  
  sessionPtr session = _server->find_session_for_username(**name);
  if (!session) {
    BOOST_LOG_TRIVIAL(error) << "no session for " << **name;
    return;
  }
  session->set_user_id(**id);
  session->send_banner();
  
  optional<json::iterator> streams = get(doc, "streams");
  if (streams) {
    if ((**streams).is_array()) {
      for (auto i: (**streams)) {
        optional<json::iterator> id = get(&i, "id");
        optional<json::iterator> name = get(&i, "name");
        optional<json::iterator> policy = get(&i, "policy");
        if (id && name && policy) {
          _server->create_channel(**name, **id, **policy);
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

void ZMQClient::policyUsersMsg(json *doc) {

  optional<json::iterator> id = get(doc, "id");
  if (!id) {
    BOOST_LOG_TRIVIAL(error) << "no id for policyusers";
    return;
  }
  channelPtr channel = _server->find_channel_policy(**id);
  if (!channel) {
    BOOST_LOG_TRIVIAL(error) << "no channel for policy " << **id;
    return;
  }
  optional<json::iterator> users = get(doc, "users");
  if (users) {
    if ((**users).is_array()) {
      for (auto i: (**users)) {
        optional<json::iterator> id = get(&i, "id");
        optional<json::iterator> name = get(&i, "name");
        optional<json::iterator> fullname = get(&i, "fullname");
        if (id && name && fullname) {
          userPtr user = channel->find_user_id(**id);
          if (!user) {
            user = userPtr(new User(**id, **name,  **fullname));
            _server->add_user(user);
            channel->join(user);
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

void ZMQClient::messageMsg(json *doc) {

  optional<json::iterator> stream = get(doc, "stream");
  optional<json::iterator> userid = get(doc, "user");
  optional<json::iterator> text = get(doc, "text");
  if (stream && userid && text) {
    channelPtr channel = _server->find_channel_stream(**stream);
    if (channel) {
      userPtr user = _server->find_user_id(**userid);
      if (user) {
        channel->send_message(user, **text);
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

void ZMQClient::ackMsg(json *doc) {

  BOOST_LOG_TRIVIAL(info) << "acknowleged";

}

void ZMQClient::errMsg(json *doc) {

  BOOST_LOG_TRIVIAL(error) << "err" << *doc;

}
