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
#include <mutex>

using namespace std;

class User;
class Prefixable;
class Server;
class Channel;

typedef shared_ptr<User> userPtr;
typedef shared_ptr<Channel> channelPtr;

class Channel : public enable_shared_from_this<Channel> {

public:

  Channel(Server *server, const string &name, const string &id, const string &policy);
  
  string id();
  string policy();

  userPtr find_user_id(const string &id);
  void send(Prefixable *prefix, const string &cmd, const list<string> &args);

  // thread safe
  void join(userPtr user);
  void send_message(userPtr user, const string &text);
  
private:
  friend class Server;
  friend class Session;
  
  Server *_server;
  
  string _name;
  string _id;
  string _policy;

  vector<userPtr > _users;
  mutex _users_mutex;  
  
};

#endif // H_channel
