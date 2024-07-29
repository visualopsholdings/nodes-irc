/*
  nodes.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 10-May-2024
    
  Dummy Nodes for testing
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes-irc
*/

#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <zmq.hpp>
#include <boost/json.hpp>
#include <vector>
#include <boost/algorithm/string/join.hpp>

using namespace std;
using json = boost::json::value;

typedef function<void (json *json, zmq::socket_t *socket)> msgHandler;
void certsMsg(json *json, zmq::socket_t *socket);
void loginMsg(json *json, zmq::socket_t *socket);
void streamsMsg(json *json, zmq::socket_t *socket);
void policyUsersMsg(json *json, zmq::socket_t *socket);
void messageMsg(json *json, zmq::socket_t *socket);
void send(zmq::socket_t *socket, const string &m);
bool getString(json *j, const string &name, string *value);

int main(int argc, char *argv[]) {

  int pubPort = 3012;
  int repPort = 3013;

//  boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
  boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
  boost::log::formatter logFmt =
         boost::log::expressions::format("%1%\tNodes\t[%2%]\t%3%")
        %  boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") 
        %  boost::log::expressions::attr< boost::log::trivial::severity_level>("Severity")
        %  boost::log::expressions::smessage;
  boost::log::add_common_attributes();
  boost::log::add_console_log(clog)->set_formatter(logFmt);

  BOOST_LOG_TRIVIAL(info) << "Nodes (Test) 0.2, 22-May-2024.";

  zmq::context_t context (1);

  zmq::socket_t pub(context, ZMQ_PUB);
  pub.bind("tcp://127.0.0.1:" + to_string(pubPort));
	BOOST_LOG_TRIVIAL(info) << "Connect to ZMQ as Local PUB on " << pubPort;

  zmq::socket_t rep(context, ZMQ_REP);
  rep.bind("tcp://127.0.0.1:" + to_string(repPort));
	BOOST_LOG_TRIVIAL(info) << "Connect to ZMQ as Local REP on " << repPort;

  map<string, msgHandler> handlers;
  handlers["certs"] = bind(&certsMsg, placeholders::_1, placeholders::_2);
  handlers["login"] = bind(&loginMsg, placeholders::_1, placeholders::_2);
  handlers["streams"] = bind(&streamsMsg, placeholders::_1, placeholders::_2);
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
        json doc = boost::json::parse(r);

        BOOST_LOG_TRIVIAL(debug) << "got reply " << doc;

        string type;
        if (!getString(&doc, "type", &type)) {
          BOOST_LOG_TRIVIAL(error) << "no type";
          continue;
        }

        map<string, msgHandler>::iterator handler = handlers.find(type);
        if (handler == handlers.end()) {
          BOOST_LOG_TRIVIAL(error) << "unknown msg type " << type;
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

void send(zmq::socket_t *socket, const json &j) {

  stringstream ss;
  ss << j;
  string m = ss.str();
  
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

bool getString(json *j, const string &name, string *value) {

  try {
    *value = boost::json::value_to<string>(j->at(name));
    return true;
  }
  catch (const boost::system::system_error& ex) {
    return false;
  }

}

void certsMsg(json *j, zmq::socket_t *socket) {

  send(socket, { { "type", "certs" }, { "ssl", false } });

}

void loginMsg(json *j, zmq::socket_t *socket) {

  string session;
  if (!getString(j, "session", &session)) {
    BOOST_LOG_TRIVIAL(error) << "no session";
    return;
  }
  string password;
  if (!getString(j, "password", &password)) {
    BOOST_LOG_TRIVIAL(error) << "no password";
    return;
  }
  if (password == "tracy") {
    send(socket, {
      { "type", "user" },
      { "session", session },
      { "id", "u1" },
      { "name", "tracy" },
      { "fullname", "Tracy" }
    });
    return;
  }
  if (password == "leanne") {
    send(socket, {
      { "type", "user" },
      { "session", session },
      { "id", "u2" },
      { "name", "leanne" },
      { "fullname", "Leanne" }
    });
    return;
  }
  send(socket, { { "type", "err" }, { "msg", "User not found" } });

}

void streamsMsg(json *j, zmq::socket_t *socket) {

  string user;
  if (!getString(j, "user", &user)) {
    BOOST_LOG_TRIVIAL(error) << "no user";
    return;
  }
  send(socket, {
    { "type", "streams" },
    { "user", user },
    { "streams", {
      { { "name", "My Conversation 1" }, { "id", "s1" }, { "policy", "p1" } },
      { { "name", "My Conversation 2" }, { "id", "s2" }, { "policy", "p2" } }
      } 
    }
  });

}

void policyUsersMsg(json *j, zmq::socket_t *socket) {

  string policy;
  if (!getString(j, "policy", &policy)) {
    BOOST_LOG_TRIVIAL(error) << "no policy";
    return;
  }
  if (policy != "p1") {
    send(socket, { { "type", "err" }, { "msg", "not correct policy" } });
    return;
  }
  send(socket, {
    { "type", "policyusers" },
    { "id", "p1" },
    { "users", {
      { { "id", "u1" }, { "name", "tracy" }, { "fullname", "Tracy" } },
      { { "id", "u2" }, { "name", "leanne" }, { "fullname", "Leanne" } }  
      } 
    }
  });
  
}

void messageMsg(json *j, zmq::socket_t *socket) {

  string user;
  if (!getString(j, "user", &user)) {
    BOOST_LOG_TRIVIAL(error) << "no user";
    return;
  }
  string stream;
  if (!getString(j, "stream", &stream)) {
    BOOST_LOG_TRIVIAL(error) << "no stream";
    return;
  }
  if (stream != "s1") {
    send(socket, { { "type", "err" }, { "msg", "not correct stream" } });
    return;
  }
  string policy;
  if (!getString(j, "policy", &policy)) {
    BOOST_LOG_TRIVIAL(error) << "no policy";
    return;
  }
  if (policy != "p1") {
    send(socket, { { "type", "err" }, { "msg", "policy" } });
    return;
  }
  string text;
  if (!getString(j, "text", &text)) {
    BOOST_LOG_TRIVIAL(error) << "no text";
    return;
  }
  if (text == "hello") {
    send(socket, {
      { "type", "message" },
      { "text", "world" },
      { "stream", "s1" },
      { "policy", "p1" },
      { "user", user == "u1" ? "u2": "u1" }
    });
    return;
  }
  BOOST_LOG_TRIVIAL(info) << "got " << text << " from " << user;
  send(socket, { { "type", "ack" } });

}
