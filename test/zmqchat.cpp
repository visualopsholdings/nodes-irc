/*
  zmqchat.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 10-May-2024
    
  Dummy ZMQCHAT for testing
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include <boost/algorithm/string/join.hpp>

using namespace std;
using json = nlohmann::json;

typedef function<void (json *json, zmq::socket_t *socket)> msgHandler;
void loginMsg(json *json, zmq::socket_t *socket);
void policyUsersMsg(json *json, zmq::socket_t *socket);
void messageMsg(json *json, zmq::socket_t *socket);
optional<json::iterator> get(json *json, const string &name);
void send(zmq::socket_t *socket, const string &m);

int main(int argc, char *argv[]) {

  int pubPort = 3012;
  int repPort = 3013;

//  boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
  boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
  boost::log::formatter logFmt =
         boost::log::expressions::format("%1%\t[%2%]\t%3%")
        %  boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") 
        %  boost::log::expressions::attr< boost::log::trivial::severity_level>("Severity")
        %  boost::log::expressions::smessage;
  boost::log::add_common_attributes();
  boost::log::add_console_log(clog)->set_formatter(logFmt);

  BOOST_LOG_TRIVIAL(info) << "ZMQCHAT (Test) 0.1, 10-May-2024.";

  zmq::context_t context (1);

  zmq::socket_t pub(context, ZMQ_PUB);
  pub.bind("tcp://127.0.0.1:" + to_string(pubPort));
	BOOST_LOG_TRIVIAL(info) << "Connect to ZMQ as Local PUB on " << pubPort;

  zmq::socket_t rep(context, ZMQ_REP);
  rep.bind("tcp://127.0.0.1:" + to_string(repPort));
	BOOST_LOG_TRIVIAL(info) << "Connect to ZMQ as Local REP on " << repPort;

  map<string, msgHandler> handlers;
  handlers["login"] = bind(&loginMsg, placeholders::_1, placeholders::_2);
  handlers["policyusers"] = bind(&policyUsersMsg, placeholders::_1, placeholders::_2 );
  handlers["message"] = bind(&messageMsg, placeholders::_1, placeholders::_2);

  zmq::pollitem_t items [] = {
      { rep, 0, ZMQ_POLLIN, 0 }
  };
  const std::chrono::milliseconds timeout{500};
  while (1) {
  
//      BOOST_LOG_TRIVIAL(debug) << "polling for messages";
    zmq::message_t message;
    zmq::poll(&items[0], 1, timeout);
  
    if (items[0].revents & ZMQ_POLLIN) {
      BOOST_LOG_TRIVIAL(debug) << "got rep message";
      zmq::message_t reply;
      try {
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
        rep.recv(&reply);
#else
        auto res = rep.recv(reply, zmq::recv_flags::none);
#endif
        // convert to JSON
        string r((const char *)reply.data(), reply.size());
        json doc = json::parse(r);

        BOOST_LOG_TRIVIAL(debug) << "got reply " << doc;

        // switch the handler based on the message type.
        optional<json::iterator> type = get(&doc, "type");
        if (!type) {
          BOOST_LOG_TRIVIAL(error) << "no type";
          continue;
        }
        map<string, msgHandler>::iterator handler = handlers.find(**type);
        if (handler == handlers.end()) {
          BOOST_LOG_TRIVIAL(error) << "unknown reply type " << **type;
          continue;
        }
        handler->second(&doc, &rep);
      }
      catch (zmq::error_t &e) {
        BOOST_LOG_TRIVIAL(warning) << "got exc with rep recv" << e.what() << "(" << e.num() << ")";
      }
    }
  }

}

string jobj(const vector<string> &nvps) {
  return "{ " + boost::algorithm::join(nvps, ", ") + " }";
}

string jnvps(const string &name, const string &val) {
  return "\"" + name + "\": \"" + val + "\"";
}

string jarray(const string &name, const vector<string> &objs) {
  return "\"" + name + "\": [ " + boost::algorithm::join(objs, ", ") + " ]";
}

void loginMsg(json *json, zmq::socket_t *socket) {

  optional<json::iterator> username = get(json, "username");
  if (!username) {
    BOOST_LOG_TRIVIAL(error) << "no username";
    return;
  }
  if (**username == "tracy") {
    send(socket, jobj({ jnvps("type", "user"), jnvps("id", "u1"), jnvps("name", "tracy"), 
        jarray("streams", { 
          jobj({ jnvps("name", "My Conversation"),  jnvps("id", "s1"),  jnvps("id", "s1"),  jnvps("policy", "p1")})
        })
      }));
    return;
  }
  if (**username == "leanne") {
    send(socket, jobj({ jnvps("type", "user"), jnvps("id", "u2"), jnvps("name", "leanne"), 
        jarray("streams", { 
          jobj({ jnvps("name", "My Conversation"),  jnvps("id", "s1"),  jnvps("id", "s1"),  jnvps("policy", "p1")})
        })
      }));
    return;
  }
  send(socket, jobj({ jnvps("type", "err"),  jnvps("msg", "User not found") }));
}

void policyUsersMsg(json *json, zmq::socket_t *socket) {

  optional<json::iterator> policy = get(json, "policy");
  if (!policy) {
    BOOST_LOG_TRIVIAL(error) << "no policy";
    return;
  }
  if (**policy != "p1") {
    send(socket, jobj({ jnvps("type", "err"),  jnvps("msg", "not correct policy") }));
    return;
  }
  send(socket, jobj({ jnvps("type", "policyusers"), jnvps("id", "p1"), 
      jarray("users", { 
        jobj({ jnvps("id", "u1"),  jnvps("name", "tracy"),  jnvps("fullname", "Tracy")}),
        jobj({ jnvps("id", "u2"),  jnvps("name", "leanne"),  jnvps("fullname", "Leanne")})
      })
    }));
  
}

void messageMsg(json *json, zmq::socket_t *socket) {

  optional<json::iterator> user = get(json, "user");
  if (!user) {
    BOOST_LOG_TRIVIAL(error) << "no user";
    return;
  }
  if (**user != "u1" && **user != "u2") {
    send(socket, jobj({ jnvps("type", "err"),  jnvps("msg", "not correct user") }));
    return;
  }
  optional<json::iterator> stream = get(json, "stream");
  if (!stream) {
    BOOST_LOG_TRIVIAL(error) << "no stream";
    return;
  }
  if (**stream != "s1") {
    send(socket, jobj({ jnvps("type", "err"),  jnvps("msg", "not correct stream") }));
    return;
  }
  optional<json::iterator> policy = get(json, "policy");
  if (!policy) {
    BOOST_LOG_TRIVIAL(error) << "no policy";
    return;
  }
  if (**policy != "p1") {
    send(socket, jobj({ jnvps("type", "err"),  jnvps("msg", "not correct policy") }));
    return;
  }
  optional<json::iterator> text = get(json, "text");
  if (!text) {
    BOOST_LOG_TRIVIAL(error) << "no text";
    return;
  }
  if (**text == "hello") {
    send(socket, jobj({ jnvps("type", "message"), jnvps("text", "world"), 
      jnvps("stream", "s1"), jnvps("policy", "p1"), 
      jnvps("user", (string)**user == "u1" ? "u2": "u1")}));
      return;
  }
  BOOST_LOG_TRIVIAL(info) << "got " << **text << " from " << **user;
  send(socket, jobj({ jnvps("type", "ack") }));

}

optional<json::iterator> get(json *json, const string &name) {

  json::iterator i = json->find(name);
  if (i == json->end()) {
    return nullopt;
  }
  return i;
  
}

void send(zmq::socket_t *socket, const string &m) {

  BOOST_LOG_TRIVIAL(info) << "sending " << m;

	zmq::message_t msg(m.length());
	memcpy(msg.data(), m.c_str(), m.length());
  try {
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
    socket->send(msg);
#else
    socket->send(msg, zmq::send_flags::none);
#endif
  }
  catch (zmq::error_t &e) {
    BOOST_LOG_TRIVIAL(warning) << "got exc send" << e.what() << "(" << e.num() << ")";
  }

}
