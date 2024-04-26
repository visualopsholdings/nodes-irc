/*
  channel.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Apr-2024
    
  A channel for ZMQ to IRC integration.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#ifndef H_channel
#define H_channel

#include <string>
#include <boost/enable_shared_from_this.hpp>

class Channel : public boost::enable_shared_from_this<Channel> {

public:

  static boost::shared_ptr<Channel> create(const std::string &name, const std::string &id, const std::string &policy);

  static std::string from_stream_name(const std::string &stream);
  static std::string normalise(const std::string &chan);
  
private:
  friend class Server;
  
  std::string _name;
  std::string _id;
  std::string _policy;
  
  explicit Channel(const std::string &name, const std::string &id, const std::string &policy);
  
};

#endif // H_channel
