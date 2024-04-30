/*
  channel.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Apr-2024
    
  A channel for ZMQ to IRC integration.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#ifndef H_channel
#define H_channel

#include <string>
#include <vector>
#include <list>

class User;
class Prefixable;
class Server;

class Channel : public std::enable_shared_from_this<Channel> {

public:

  static std::shared_ptr<Channel> create(Server *server, const std::string &name, const std::string &id, const std::string &policy);

  static std::string from_stream_name(const std::string &stream);
  static std::string normalise(const std::string &chan);
  
  void join(std::shared_ptr<User> user);
  std::shared_ptr<User> find_user_id(const std::string &id);
  void add_user(std::shared_ptr<User> user);
  void send(Prefixable *prefix, const std::string &cmd, const std::list<std::string> &args);
  
private:
  friend class Server;
  friend class Session;
  
  Server *_server;
  
  std::string _name;
  std::string _id;
  std::string _policy;

  std::vector<std::shared_ptr<User> > _users;
  
  explicit Channel(Server *server, const std::string &name, const std::string &id, const std::string &policy);
  
};

#endif // H_channel
