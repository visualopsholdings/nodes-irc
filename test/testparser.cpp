/*
  testparser.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 18-Apr-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include <iostream>

#include "parser.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( emptyString )
{
  cout << "=== emptyString ===" << endl;
  
  vector<string> args;
  BOOST_CHECK(Parser::parse("", &args) == "");
  BOOST_CHECK(args.size() == 0);

}

BOOST_AUTO_TEST_CASE( CAP )
{
  cout << "=== CAP ===" << endl;
  
  vector<string> args;
  BOOST_CHECK(Parser::parse("CAP LS", &args) == "CAP");
  BOOST_CHECK(args.size() == 1);
  BOOST_CHECK(args.front() == "LS");
  
}

BOOST_AUTO_TEST_CASE( NICK )
{
  cout << "=== NICK ===" << endl;
  
  vector<string> args;
  BOOST_CHECK(Parser::parse("NICK nickname", &args) == "NICK");
  BOOST_CHECK(args.size() == 1);
  BOOST_CHECK(args.front() == "nickname");
  
}

BOOST_AUTO_TEST_CASE( USER )
{
  cout << "=== USER ===" << endl;
  
  vector<string> args;
  BOOST_CHECK(Parser::parse("USER username servername localhost :RealName", &args) == "USER");
  BOOST_CHECK(args.size() == 4);
  BOOST_CHECK(args[0] == "username");
  BOOST_CHECK(args[1] == "servername");
  BOOST_CHECK(args[2] == "localhost");
  BOOST_CHECK(args[3] == ":RealName");
  
}
