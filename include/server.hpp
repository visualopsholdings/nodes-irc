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
#include <boost/optional.hpp>
#include <nlohmann/json.hpp>

class Session;
class Channel;

class Server {

public:
  Server(zmq::socket_t *sub, zmq::socket_t *req);
  
  void run();
  void login(const std::string &username);
    
private:
  boost::asio::io_service _io_service;
  boost::asio::ip::tcp::acceptor _acceptor;
  zmq::socket_t *_sub;
  zmq::socket_t *_req;
  std::vector<boost::shared_ptr<Session> > _sessions;
  std::vector<boost::shared_ptr<Channel> > _channels;
   
  void start_accept();
  void handle_accept(boost::shared_ptr<Session> session,
      const boost::system::error_code& error);
  boost::optional<nlohmann::json::iterator> get(nlohmann::json *json, const std::string &name);
  boost::shared_ptr<Session> find_session_username(const std::string &username);
  void create_channel(const std::string &name, const std::string &id, const std::string &policy);
  
};

#endif // H_server
