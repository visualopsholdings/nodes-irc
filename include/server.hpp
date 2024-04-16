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

#include <zmq.hpp>

class Server {

public:
  Server(zmq::socket_t *sub, zmq::socket_t *req):
    _sub(sub), _req(req) {}
  ~Server();
  
  void run();
  
private:
  zmq::socket_t *_sub;
  zmq::socket_t *_req;

};

#endif // H_server
