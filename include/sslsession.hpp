/*
  sslsession.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 10-May-2024
    
  An SSL session for Nodes to IRC integration.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes-irc
*/

#ifndef H_sslsession
#define H_sslsession

#include "session.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

class SSLSession : public Session {

public:
  
  SSLSession(Server *server, boost::asio::io_service& io_service, boost::asio::ssl::context& context);
  virtual ~SSLSession() {};

  virtual void start();
  virtual boost::asio::basic_socket<boost::asio::ip::tcp, boost::asio::any_io_executor> &socket();
  virtual void read();
  virtual void write(const string &line);

  static void setup(boost::asio::ssl::context *context, const string &chain, const string &cert);
  
protected:
  ssl_socket _socket;

private:
  void handle_handshake(const boost::system::error_code& error);

};

#endif // H_sslsession
