/*
  channel.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Apr-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include "channel.hpp"

#include <boost/algorithm/string.hpp>
#include <regex>

using namespace std;

Channel::Channel(const string &name, const string &id, const string &policy) :
	_name(name), _id(id), _policy(policy) {
}
	
boost::shared_ptr<Channel> Channel::create(const string &name, const string &id, const string &policy) {

	return boost::shared_ptr<Channel>(new Channel(name, id, policy));

}
string Channel::normalise(const std::string &chan) {

  string s = chan;
  boost::trim(s);
  boost::to_lower(s);
  // TBD: {, } and | are uppercase variants of [, ] and \ respectively
  return s;
}

string Channel::from_stream_name(const std::string &stream) {

  string name = "#" + stream;
  regex r(" ");
  return normalise(regex_replace(name, r, "+"));

}
