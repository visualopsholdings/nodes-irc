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

using namespace std;

class Channel;
class User;

typedef shared_ptr<User> userPtr;

class User : public enable_shared_from_this<User>, Prefixable {

public:
  ~User();
  
  static userPtr create(const string &nick);

  // Prefixable
  const string prefix();

private:
  friend class Server;
  friend class Session;
  friend class Channel;
  
  explicit User(const string &nick);

  string _nick;
  string _username;
  string _realname;
  string _id;
  
};

#endif // H_user
