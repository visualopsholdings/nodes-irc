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

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

using namespace std;

class Session;
class Channel;
class User;
class ZMQClient;

typedef shared_ptr<User> userPtr;
typedef shared_ptr<Channel> channelPtr;
typedef shared_ptr<Session> sessionPtr;
typedef shared_ptr<ZMQClient> zmqClientPtr;

class Server : public Prefixable {

public:
  Server(const string &version, int sub, int req, int port);
  ~Server();
  
  void start();
  void run();
  void run(const string &certFile, const string &chainFile);
  userPtr find_user_id(const string &id);
  userPtr find_user_nick(const string &nick);
  sessionPtr find_session(const string &sessionid);
  sessionPtr find_session_for_nick(const string &nick);
  void remove_session(sessionPtr session);
  
  // thread safe
  void create_channel(const string &name, const string &id, const string &policy);
  void add_user(userPtr user);
  void channel_names(vector<string> *names);
  channelPtr find_channel(const string &name);
  channelPtr find_channel_policy(const string &policy);
  channelPtr find_channel_stream(const string &stream);
    
  // Prefixable
  string prefix();

  zmqClientPtr _zmq;
  string _version;

private:

  boost::asio::io_service _io_service;
  boost::asio::ip::tcp::acceptor _acceptor;
  vector<sessionPtr > _sessions;
  vector<channelPtr > _channels;
  mutex _channels_mutex;
  vector<userPtr > _users;
  mutex _users_mutex;
  boost::asio::ssl::context _context;
  bool _ssl;
  
  void start_accept();
  void handle_accept(sessionPtr session,
      const boost::system::error_code& error);
  channelPtr find_channel_(const string &name);
  
};

#endif // H_server
