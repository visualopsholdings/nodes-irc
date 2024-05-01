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

#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <map>
#include <list>

using namespace std;

class Server;
class Channel;
class User;
class Prefixable;
class Session;

typedef shared_ptr<Session> sessionPtr;
typedef shared_ptr<Channel> channelPtr;
typedef shared_ptr<User> userPtr;

typedef function<void (const vector<string> &args)> cmdHandler;

class Session : public enable_shared_from_this<Session> {

public:
  
  Session(Server *server, boost::asio::io_service& io_service);

  // thread safe
  void set_user_id(const string &id);
  void send_banner();
  void send(Prefixable *prefix, const string &cmd, const list<string> &args);
  
private:
  friend class Server;
  
  Server *_server;
  boost::asio::ip::tcp::socket _socket;
  mutex _socket_mutex;  
  boost::asio::streambuf _buffer;
  map<string, cmdHandler> _commands;
  userPtr _user;
  mutex _user_mutex;  
  
  void start();
  void read();
  void handle_read(const boost::system::error_code& error,
      const size_t bytes_transferred);
  void write(const string &line);
  void handle_write(const boost::system::error_code& error);
  void handle_request();
  
  // command handlers
  void capCmd(const vector<string> &args);
  void nickCmd(const vector<string> &args);
  void userCmd(const vector<string> &args);
  void listCmd(const vector<string> &args);
  void joinCmd(const vector<string> &args);
  void msgCmd(const vector<string> &args);
  void whoCmd(const vector<string> &args);
  void modeCmd(const vector<string> &args);
  void quitCmd(const vector<string> &args);

};

#endif // H_session
