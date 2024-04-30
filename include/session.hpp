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

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <vector>
#include <string>
#include <map>
#include <list>

class Server;
class Channel;
class User;
class Prefixable;

typedef boost::function<void (const std::vector<std::string> &args)> cmdHandler;

class Session : public boost::enable_shared_from_this<Session> {

public:
  
  static boost::shared_ptr<Session> create(Server *server,
      boost::asio::io_service& io_service);

  void start();
  void join(boost::shared_ptr<Channel> channel);
  void send(Prefixable *prefix, const std::string &cmd, const std::list<std::string> &args);
  
private:
  friend class Server;
  
  Server *_server;
  boost::asio::ip::tcp::socket _socket;
  boost::asio::streambuf _buffer;
  std::map<std::string, cmdHandler> _commands;
  boost::shared_ptr<User> _user;
  
  explicit Session(Server *server, boost::asio::io_service& io_service);

  void handle_read(const boost::system::error_code& error,
      const std::size_t bytes_transferred);
  void handle_write(const boost::system::error_code& error);

  void write(const std::string &line);
  void login(const std::string &username);
  
  void handle_request();
  
  // command handlers
  void nickCmd(const std::vector<std::string> &args);
  void userCmd(const std::vector<std::string> &args);
  void listCmd(const std::vector<std::string> &args);
  void joinCmd(const std::vector<std::string> &args);
  void msgCmd(const std::vector<std::string> &args);

};

#endif // H_session
