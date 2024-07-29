/*
  zmqclient.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 1-May-2024
    
  A ZMQ Client for Nodes to IRC integration.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes-irc
*/

#ifndef H_zmqclient
#define H_zmqclient

#include <string>
#include <map>
#include <boost/json.hpp>
#include <zmq.hpp>

using namespace std;
using json = boost::json::value;

class Server;
class User;
class Channel;

typedef shared_ptr<User> userPtr;
typedef shared_ptr<Channel> channelPtr;
typedef function<void (json *)> msgHandler;

class ZMQClient : public enable_shared_from_this<ZMQClient> {

public:
  ZMQClient(Server *server, int sub, int req);
  
  void run();
  void certs();
  void login(const string &session, const string &password);
  void streams(const string &user);
  void policy_users(const string &policy);
  void send(userPtr user, channelPtr channel, const string &text);
  void receive1();
  
private:
  Server *_server;
  shared_ptr<zmq::context_t> _context;
  shared_ptr<zmq::socket_t> _sub;
  shared_ptr<zmq::socket_t> _req;
  map<string, msgHandler> _reqmessages;
  map<string, msgHandler> _submessages;
  
  void receive();
  
  static bool getString(json *j, const string &name, string *value);
  static bool getBool(json *j, const string &name, bool *value);
  static void handle_reply(const zmq::message_t &reply, map<string, msgHandler> *handlers);
  void send(const json &j);
  bool trySend(const string &m);
  
  // msg handlers
  void certsMsg(json *);
  void userMsg(json *);
  void streamsMsg(json *);
  void policyUsersMsg(json *);
  void messageMsg(json *);
  void ackMsg(json *);
  void errMsg(json *);
  
};

#endif // H_zmqclient
