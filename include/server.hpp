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

class Session;
class Channel;
class User;

class Server : public Prefixable {

public:
  Server(zmq::socket_t *sub, zmq::socket_t *req);
  ~Server();
  
  void run();
  void login(const std::string &username);
  void policy_users(const std::string &policy);
  std::shared_ptr<Channel> find_channel(const std::string &name);
  std::vector<std::shared_ptr<Channel> >::iterator begin_channel();
  std::vector<std::shared_ptr<Channel> >::iterator end_channel();
  void create_channel(const std::string &name, const std::string &id, const std::string &policy);
  std::shared_ptr<Channel> find_channel_policy(const std::string &policy);
  std::shared_ptr<Channel> find_channel_stream(const std::string &stream);
  std::shared_ptr<User> find_user_id(const std::string &id);
  std::shared_ptr<User> find_user_nick(const std::string &nick);
  void add_user(std::shared_ptr<User> user);
  std::shared_ptr<Session> find_session_for_nick(const std::string &nick);
  void send(std::shared_ptr<User> user, std::shared_ptr<Channel> channel, const std::string &text);
  
  // Prefixable
  const std::string prefix();

private:
  
  boost::asio::io_service _io_service;
  boost::asio::ip::tcp::acceptor _acceptor;
  zmq::socket_t *_sub;
  zmq::socket_t *_req;
  std::vector<std::shared_ptr<Session> > _sessions;
  std::vector<std::shared_ptr<Channel> > _channels;
  std::vector<std::shared_ptr<User> > _users;
   
  void start_accept();
  void handle_accept(std::shared_ptr<Session> session,
      const boost::system::error_code& error);
  std::optional<json::iterator> get(json *json, const std::string &name);
  std::shared_ptr<Session> find_session_username(const std::string &username);
  
};

#endif // H_server
