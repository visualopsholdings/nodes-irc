/*
  session.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Apr-2024
    
  A session for Nodes to IRC integration.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes-irc
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
typedef list<pair<const string, const list<string> > > itemsType;

class Session : public enable_shared_from_this<Session> {

public:
  
  Session(Server *server, boost::asio::io_service& io_service);
  virtual ~Session() {};
  
  void set_id(const string &id);

  // thread safe
  void set_user_details(const string &id, const string &name, const string &fullname);
  void send_banner();
  void send(Prefixable *prefix, itemsType &items);
  void send_error(const string &err, const string &msg);
  
  virtual void start();
  virtual boost::asio::basic_socket<boost::asio::ip::tcp, boost::asio::any_io_executor> &socket() = 0;
  virtual void read() = 0;
  virtual void write(const string &line) = 0;

protected:
  boost::asio::streambuf _buffer;
   
  void handle_read(const boost::system::error_code& error, const size_t bytes_transferred);
  void handle_write(const boost::system::error_code& error);

private:
  friend class Server;
  
  string _id;
  Server *_server;
  mutex _socket_mutex;  
  map<string, cmdHandler> _commands;
  userPtr _user;
  mutex _user_mutex;  
  
  void handle_request();
  
  // command handlers
  void capCmd(const vector<string> &args);
  void passCmd(const vector<string> &args);
  void listCmd(const vector<string> &args);
  void joinCmd(const vector<string> &args);
  void msgCmd(const vector<string> &args);
  void whoCmd(const vector<string> &args);
  void modeCmd(const vector<string> &args);
  void quitCmd(const vector<string> &args);

};

#endif // H_session
