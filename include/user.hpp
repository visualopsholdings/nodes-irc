/*
  user.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Apr-2024
    
  A user for Nodes to IRC integration.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes-irc
*/

#ifndef H_user
#define H_user

#include "prefixable.hpp"

#include <string>
#include <memory>

using namespace std;

class Channel;
class User;

typedef shared_ptr<User> userPtr;

class User : public enable_shared_from_this<User>, public Prefixable {

public:
  User(const string &id, const string &nick, const string &realname);
  
  string id();
  
  // Prefixable
  string prefix();

private:
  friend class Server;
  friend class Session;
  friend class Channel;
  
  string _nick;
  string _realname;
  string _id;
  
};

#endif // H_user
