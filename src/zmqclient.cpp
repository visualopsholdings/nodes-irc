/*
  zmqclient.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 1-May-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes-irc
*/

#include "zmqclient.hpp"

#include "session.hpp"
#include "user.hpp"
#include "vecutil.hpp"
#include "server.hpp"
#include "channel.hpp"

#include <boost/log/trivial.hpp>
#include <thread>

ZMQClient::ZMQClient(Server *server, int subPort, int reqPort) : 
  _server(server) {

  _context.reset(new zmq::context_t(1));
  _sub.reset(new zmq::socket_t(*_context, ZMQ_SUB));
  _sub->connect("tcp://127.0.0.1:" + to_string(subPort));
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)  
  _sub->setsockopt(ZMQ_SUBSCRIBE, 0, 0);
#else
  _sub->set(zmq::sockopt::subscribe, "");
#endif
	BOOST_LOG_TRIVIAL(info) << "Connect to ZMQ as Local SUB on " << subPort;

  _req.reset(new zmq::socket_t(*_context, ZMQ_REQ));
  _req->connect("tcp://127.0.0.1:" + to_string(reqPort));
	BOOST_LOG_TRIVIAL(info) << "Connect to ZMQ as Local REQ on " << reqPort;
  
  // expect these as replies
  _reqmessages["certs"] = bind( &ZMQClient::certsMsg, this, placeholders::_1 );
  _reqmessages["user"] = bind( &ZMQClient::userMsg, this, placeholders::_1 );
  _reqmessages["streams"] = bind( &ZMQClient::streamsMsg, this, placeholders::_1 );
  _reqmessages["policyusers"] = bind( &ZMQClient::policyUsersMsg, this, placeholders::_1 );
  _reqmessages["idea"] = bind( &ZMQClient::ideaMsg, this, placeholders::_1 );
  _reqmessages["ack"] = bind( &ZMQClient::ackMsg, this, placeholders::_1 );
  _reqmessages["err"] = bind( &ZMQClient::errMsg, this, placeholders::_1 );
  
  // expect these being published
  _submessages["idea"] = bind( &ZMQClient::ideaMsg, this, placeholders::_1 );

}

void ZMQClient::run() {

  thread t(bind(&ZMQClient::receive, this));
  t.detach();
  
}

void ZMQClient::receive1() {

  zmq::message_t reply;
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
  _req->recv(&reply);
#else
  auto res = _req->recv(reply, zmq::recv_flags::none);
#endif
  handle_reply(reply, &_reqmessages);

}

void ZMQClient::receive() {

  BOOST_LOG_TRIVIAL(trace) << "start receiving";

  zmq::pollitem_t items [] = {
      { *_sub, 0, ZMQ_POLLIN, 0 },
      { *_req, 0, ZMQ_POLLIN, 0 }
  };
  const std::chrono::milliseconds timeout{500};
  while (1) {
    
//    BOOST_LOG_TRIVIAL(debug) << "polling for messages";
    zmq::message_t message;
    zmq::poll(items, 2, timeout);

//     BOOST_LOG_TRIVIAL(trace) << "sub events " << items[0].revents;
//     BOOST_LOG_TRIVIAL(trace) << "req events " << items[1].revents;

    if (items[0].revents & ZMQ_POLLIN) {
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
    if (items[1].revents & ZMQ_POLLIN) {
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
  }

}

void ZMQClient::handle_reply(const zmq::message_t &reply, map<string, msgHandler> *handlers) {

  BOOST_LOG_TRIVIAL(trace) << "handling reply";

  // convert to JSON
  string r((const char *)reply.data(), reply.size());
  json doc = boost::json::parse(r);

  BOOST_LOG_TRIVIAL(debug) << "got reply " << doc;

  // switch the handler based on the message type.
  string type;
  if (!getString(&doc, "type", &type)) {
    BOOST_LOG_TRIVIAL(error) << "no type";
    return;
  }

  map<string, msgHandler>::iterator handler = handlers->find(type);
  if (handler == handlers->end()) {
    BOOST_LOG_TRIVIAL(error) << "unknown reply type " << type;
    return;
  }
  handler->second(&doc);
  
}

bool ZMQClient::trySend(const string &m) {

  BOOST_LOG_TRIVIAL(trace) << "try sending " << m;

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

void ZMQClient::send(const json &j) {

  stringstream ss;
  ss << j;
  string m = ss.str();
  
  if (!trySend(m)) {
    for (int i=0; i<4; i++) {
	    this_thread::sleep_for(chrono::milliseconds(20));
      BOOST_LOG_TRIVIAL(trace) << "retrying send";
	    if (trySend(m)) {
	      return;
	    }
    }
  }

}

void ZMQClient::certs() {

	send({ 
	  { "type", "certs" }
	});

}

void ZMQClient::login(const string &session, const string &password) {
  
	send({ 
	  { "type", "login" }, 
	  { "session", session }, 
	  { "password", password } 
	});

}

void ZMQClient::streams(const string &user) {
  
	send({ 
	  { "type", "streams" }, 
	  { "user", user }
	});

}

void ZMQClient::policy_users(const string &policy) {

	send({ 
	  { "type", "policyusers" }, 
	  { "policy", policy }
	});

}

void ZMQClient::send(userPtr user, channelPtr channel, const string &text) {

	send({ 
	  { "type", "message" }, 
	  { "user", user->id() },
	  { "stream", channel->id() },
	  { "policy", channel->policy() },
	  { "text", text }
	});

}

bool ZMQClient::getString(json *j, const string &name, string *value) {

  try {
    *value = boost::json::value_to<string>(j->at(name));
    return true;
  }
  catch (const boost::system::system_error& ex) {
    return false;
  }

}

bool ZMQClient::getBool(json *j, const string &name, bool *value) {

  try {
    *value = boost::json::value_to<bool>(j->at(name));
    return true;
  }
  catch (const boost::system::system_error& ex) {
    return false;
  }

}

void ZMQClient::certsMsg(json *doc) {

  bool ssl;
  if (!getBool(doc, "ssl", &ssl)) {
    BOOST_LOG_TRIVIAL(error) << "no ssl";
    return;
  }
  if (ssl) {
    string certFile;
    if (!getString(doc, "certFile", &certFile)) {
      BOOST_LOG_TRIVIAL(error) << "no certFile";
      return;
    }
    string chainFile;
    if (!getString(doc, "chainFile", &chainFile)) {
      BOOST_LOG_TRIVIAL(error) << "no chainFile";
      return;
    }
    run();
    _server->run(certFile, chainFile);
  }
  else {
    run();
    _server->run();
  }
  
}


void ZMQClient::userMsg(json *doc) {

  string sessionid;
  if (!getString(doc, "session", &sessionid)) {
    BOOST_LOG_TRIVIAL(error) << "no session for user";
    return;
  }
  
  sessionPtr session = _server->find_session(sessionid);
  if (!session) {
    BOOST_LOG_TRIVIAL(error) << "no session " << sessionid;
    return;
  }
  
  string id;
  if (!getString(doc, "id", &id)) {
    BOOST_LOG_TRIVIAL(error) << "no id for user";
    return;
  }
  string name;
  if (!getString(doc, "name", &name)) {
    BOOST_LOG_TRIVIAL(error) << "no name for user";
    return;
  }
  string fullname;
  if (!getString(doc, "fullname", &fullname)) {
    BOOST_LOG_TRIVIAL(error) << "no fullname for user";
    return;
  }
  session->set_user_details(id, name, fullname);
  session->send_banner();
  
  streams(id);

}

void ZMQClient::streamsMsg(json *doc) {

  try {
    boost::json::value v = doc->at("streams");
    boost::json::array a = v.get_array();
    for (auto i: a) {
      string id;
      if (!getString(&i, "id", &id)) {
        BOOST_LOG_TRIVIAL(error) << "stream not complete, no id";
        continue;
      }
      string name;
      if (!getString(&i, "name", &name)) {
        BOOST_LOG_TRIVIAL(error) << "stream not complete, no name";
        continue;
      }
      string policy;
      if (!getString(&i, "policy", &policy)) {
         BOOST_LOG_TRIVIAL(error) << "stream not complete, no policy";
        continue;
      }
      _server->create_channel(name, id, policy);
    }
  }
  catch (const boost::system::system_error& ex) {
    BOOST_LOG_TRIVIAL(error) << "missing streams or not array";
  }

}

void ZMQClient::policyUsersMsg(json *doc) {

  string id;
  if (!getString(doc, "id", &id)) {
    BOOST_LOG_TRIVIAL(error) << "no id for policyusers";
    return;
  }
  channelPtr channel = _server->find_channel_policy(id);
  if (!channel) {
    BOOST_LOG_TRIVIAL(error) << "no channel for policy " << id;
    return;
  }
  
  try {
    boost::json::value v = doc->at("users");
    boost::json::array a = v.get_array();
    for (auto i: a) {
      string id;
      if (!getString(&i, "id", &id)) {
        BOOST_LOG_TRIVIAL(error) << "user not complete, no id";
        continue;
      }
      string name;
      if (!getString(&i, "name", &name)) {
        BOOST_LOG_TRIVIAL(error) << "user not complete, no name";
        continue;
      }
      string fullname;
      if (!getString(&i, "fullname", &fullname)) {
         BOOST_LOG_TRIVIAL(error) << "user not complete, no fullname";
        continue;
      }
      userPtr user = channel->find_user_id(id);
      if (!user) {
        user = userPtr(new User(id, name,  fullname));
        _server->add_user(user);
        channel->join(user);
      }
    }
  }
  catch (const boost::system::system_error& ex) {
    BOOST_LOG_TRIVIAL(error) << "missing users or not array";
  }

}

void ZMQClient::ideaMsg(json *doc) {

  string stream;
  if (!getString(doc, "stream", &stream)) {
    BOOST_LOG_TRIVIAL(error) << "no stream for message";
    return;
  }
  string userid;
  if (!getString(doc, "user", &userid)) {
    BOOST_LOG_TRIVIAL(error) << "no user for message";
    return;
  }
  string text;
  if (!getString(doc, "text", &text)) {
    BOOST_LOG_TRIVIAL(error) << "no text for message";
    return;
  }
  channelPtr channel = _server->find_channel_stream(stream);
  if (channel) {
    userPtr user = _server->find_user_id(userid);
    if (user) {
      channel->send_message(user, text);
    }
    else {
      BOOST_LOG_TRIVIAL(error) << "user not found";
    }
  }
  else {
    BOOST_LOG_TRIVIAL(error) << "channel not found";
  }

}

void ZMQClient::ackMsg(json *doc) {

  BOOST_LOG_TRIVIAL(info) << "acknowleged";

}

void ZMQClient::errMsg(json *doc) {

  BOOST_LOG_TRIVIAL(error) << "err" << *doc;

}
