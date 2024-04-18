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
#include <boost/asio.hpp>

class Session;

class Server {

public:
  Server(zmq::socket_t *sub, zmq::socket_t *req);
  
  void run();
    
private:
  boost::asio::io_service _io_service;
  boost::asio::ip::tcp::acceptor _acceptor;
  zmq::socket_t *_sub;
  zmq::socket_t *_req;

  void start_accept();
  void handle_accept(boost::shared_ptr<Session> session,
      const boost::system::error_code& error);

};

#endif // H_server
