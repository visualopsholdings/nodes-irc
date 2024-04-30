/*
  server.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Apr-2024
    
  The main server for ZMQ to IRC integration.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#ifndef H_server
#define H_server

#include "prefixable.hpp"

#include <zmq.hpp>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

class Session;
class Channel;
class User;

typedef shared_ptr<User> userPtr;
typedef shared_ptr<Channel> channelPtr;
typedef shared_ptr<Session> sessionPtr;

class Server : public Prefixable {

public:
  Server(zmq::socket_t *sub, zmq::socket_t *req);
  ~Server();
  
  void run();
  userPtr find_user_id(const string &id);
  sessionPtr find_session_for_nick(const string &nick);
  void send(userPtr user, channelPtr channel, const string &text);
  
  // Prefixable
  const string prefix();

private:
  friend class Session;
  
  boost::asio::io_service _io_service;
  boost::asio::ip::tcp::acceptor _acceptor;
  zmq::socket_t *_sub;
  zmq::socket_t *_req;
  vector<sessionPtr > _sessions;
  vector<channelPtr > _channels;
  vector<userPtr > _users;
   
  void start_accept();
  void handle_accept(sessionPtr session,
      const boost::system::error_code& error);
  optional<json::iterator> get(json *json, const string &name);
  void login(const string &username);
  void policy_users(const string &policy);
  channelPtr find_channel(const string &name);
  void create_channel(const string &name, const string &id, const string &policy);
    
};

#endif // H_server
