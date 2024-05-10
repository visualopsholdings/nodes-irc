/*
  zmqirc.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Apr-2024
    
  Command line driver for ZMQ to IRC integration.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include "server.hpp"

#include <iostream>
#include <boost/program_options.hpp> 
#include <boost/log/trivial.hpp>

namespace po = boost::program_options;

using namespace std;

int main(int argc, char *argv[]) {

  int subPort;
  int reqPort;
  int port;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("subPort", po::value<int>(&subPort)->default_value(3012), "ZMQ Sub port.")
    ("reqPort", po::value<int>(&reqPort)->default_value(3013), "ZMQ Req port.")
    ("port", po::value<int>(&port)->default_value(6667), "IRC port.")
    ("help", "produce help message")
    ;
  po::positional_options_description p;

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
          options(desc).positional(p).run(), vm);
  po::notify(vm);   

  cout << "ZMQIRC 1.0, 8-May-2024." << endl;

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }
 
  zmq::context_t context (1);
  zmq::socket_t sub(context, ZMQ_SUB);
  sub.connect("tcp://127.0.0.1:" + to_string(subPort));
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)  
  sub.setsockopt(ZMQ_SUBSCRIBE, "");
#else
  sub.set(zmq::sockopt::subscribe, "");
	BOOST_LOG_TRIVIAL(info) << "Connect to ZMQ as Local SUB on " << subPort;
#endif
  zmq::socket_t req(context, ZMQ_REQ);
  req.connect("tcp://127.0.0.1:" + to_string(reqPort));
	BOOST_LOG_TRIVIAL(info) << "Connect to ZMQ as Local REQ on " << reqPort;
  
  Server server(&sub, &req, port);
  server.run();

}
