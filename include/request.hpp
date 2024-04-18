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

class Session;

class Request {

public:

  Request(Session* session);
  
  void handle();
  
private:
  Session *_session;
};

#endif // H_request
