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
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace po = boost::program_options;

using namespace std;

int main(int argc, char *argv[]) {

  int subPort;
  int reqPort;
  int port;
  string logLevel;
  string chainFile;
  string certFile;
  
  po::options_description desc("Allowed options");
  desc.add_options()
    ("subPort", po::value<int>(&subPort)->default_value(3012), "ZMQ Sub port.")
    ("reqPort", po::value<int>(&reqPort)->default_value(3013), "ZMQ Req port.")
    ("port", po::value<int>(&port)->default_value(6667), "IRC port.")
    ("logLevel", po::value<string>(&logLevel)->default_value("info"), "Logging level [trace, debug, warn, info].")
    ("chainFile", po::value<string>(&chainFile)->default_value(""), "Use SSL, this is the CERT file for the chain.")
    ("certFile", po::value<string>(&certFile)->default_value(""), "Use SSL, this is the CERT file.")
    ("help", "produce help message")
    ;
  po::positional_options_description p;

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
          options(desc).positional(p).run(), vm);
  po::notify(vm);   

  if (logLevel == "trace") {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
  }
  else if (logLevel == "debug") {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
  }
  else if (logLevel == "warn") {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
  }
  else {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
  }

  boost::log::formatter logFmt =
         boost::log::expressions::format("%1%\t[%2%]\t%3%")
        %  boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") 
        %  boost::log::expressions::attr< boost::log::trivial::severity_level>("Severity")
        %  boost::log::expressions::smessage;
  boost::log::add_common_attributes();
  boost::log::add_console_log(clog)->set_formatter(logFmt);

  BOOST_LOG_TRIVIAL(info) << "ZMQIRC 1.0, 8-May-2024.";

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
  
  Server server(&sub, &req, port, certFile, chainFile);
  server.run();

}
