/*
  request.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Apr-2024
    
  A server request for ZMQ to IRC integration.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#ifndef H_request
#define H_request

#include <list>
#include <map>

class Session;

typedef void (*cmdHandler)(Session *session, const std::list<std::string> &args);

class Request {

public:

  Request(Session* session);
  
  void handle();
  
  // command handlers
  static void nick(Session *session, const std::list<std::string> &args);
  static void user(Session *session, const std::list<std::string> &args);
  
private:
  Session *_session;
  std::map<std::string, cmdHandler> _commands;
};

#endif // H_request
