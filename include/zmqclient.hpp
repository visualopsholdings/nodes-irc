/*
  zmqclient.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 1-May-2024
    
  A ZMQ Client for ZMQ to IRC integration.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#ifndef H_zmqclient
#define H_zmqclient

#include <string>
#include <nlohmann/json.hpp>
#include <zmq.hpp>

using namespace std;
using json = nlohmann::json;

class Server;
class User;
class Channel;

typedef shared_ptr<User> userPtr;
typedef shared_ptr<Channel> channelPtr;

typedef function<void (json *)> msgHandler;

class ZMQClient : public enable_shared_from_this<ZMQClient> {

public:
  ZMQClient(Server *server, zmq::socket_t *sub, zmq::socket_t *req);
  
  void run();
  void login(const string &session, const string &password);
  void streams(const string &user);
  void policy_users(const string &policy);
  void send(userPtr user, channelPtr channel, const string &text);
  
private:
  Server *_server;
  zmq::socket_t *_sub;
  zmq::socket_t *_req;
  map<string, msgHandler> _reqmessages;
  map<string, msgHandler> _submessages;
  
  void receive();

  static optional<json::iterator> get(json *json, const string &name);
  static void handle_reply(const zmq::message_t &reply, map<string, msgHandler> *handlers);
  void send(const string &m);
  bool trySend(const string &m);
  
  // msg handlers
  void userMsg(json *);
  void streamsMsg(json *);
  void policyUsersMsg(json *);
  void messageMsg(json *);
  void ackMsg(json *);
  void errMsg(json *);
  
};

#endif // H_zmqclient
