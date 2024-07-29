/*
  tcpsession.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 11-May-2024
    
  A TCP (non SSL) session for Nodes to IRC integration.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes-irc
*/

#ifndef H_tcpsession
#define H_tcpsession

#include "session.hpp"

class TCPSession : public Session {

public:
  
  TCPSession(Server *server, boost::asio::io_service& io_service);
  virtual ~TCPSession() {};

  virtual boost::asio::basic_socket<boost::asio::ip::tcp, boost::asio::any_io_executor> &socket();
  virtual void read();
  virtual void write(const string &line);

protected:
  boost::asio::ip::tcp::socket _socket;

};

#endif // H_tcpsession
