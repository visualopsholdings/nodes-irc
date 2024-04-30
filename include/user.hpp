/*
  user.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Apr-2024
    
  A user for ZMQ to IRC integration.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#ifndef H_user
#define H_user

#include "prefixable.hpp"

#include <string>

class Channel;

class User : public std::enable_shared_from_this<User>, Prefixable {

public:
  ~User();
  
  static std::shared_ptr<User> create(const std::string &nick);

  // Prefixable
  const std::string prefix();

private:
  friend class Server;
  friend class Session;
  friend class Channel;
  
  explicit User(const std::string &nick);

  std::string _nick;
  std::string _username;
  std::string _realname;
  std::string _id;
  
};

#endif // H_user
