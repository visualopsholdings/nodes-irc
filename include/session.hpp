/*
  session.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Apr-2024
    
  A session for ZMQ to IRC integration.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#ifndef H_session
#define H_session

#include "request.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

class Server;

class Session : public boost::enable_shared_from_this<Session> {

public:
  
  static boost::shared_ptr<Session> create(Server *server,
      boost::asio::io_service& io_service);

  void start();
  
private:
  friend class Request;
  friend class Server;
  
  Server *_server;
  boost::asio::ip::tcp::socket _socket;
  boost::asio::streambuf _buffer;
  Request _request;
  
  std::string _nick;
  std::string _username;
  std::string _id;
  
  explicit Session(Server *server, boost::asio::io_service& io_service);
  
  void handle_read(const boost::system::error_code& error,
      const std::size_t bytes_transferred);
  void handle_write(const boost::system::error_code& error);

  void write(const std::string &line);
  void send(const std::string &cmd, const std::list<std::string> &args);
  void login(const std::string &username);
  
};

#endif // H_session
