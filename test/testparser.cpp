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
#include <boost/test/auto_unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( emptyString )
{
  cout << "=== emptyString ===" << endl;
  
  list<string> args;
  BOOST_CHECK(Parser::parse("", &args) == "");
  BOOST_CHECK(args.size() == 0);

}

BOOST_AUTO_TEST_CASE( CAP )
{
  cout << "=== CAP ===" << endl;
  
  list<string> args;
  BOOST_CHECK(Parser::parse("CAP LS", &args) == "CAP");
  BOOST_CHECK(args.size() == 1);
  BOOST_CHECK(args.front() == "LS");
  
}

BOOST_AUTO_TEST_CASE( NICK )
{
  cout << "=== NICK ===" << endl;
  
  list<string> args;
  BOOST_CHECK(Parser::parse("NICK nickname", &args) == "NICK");
  BOOST_CHECK(args.size() == 1);
  BOOST_CHECK(args.front() == "nickname");
  
}

BOOST_AUTO_TEST_CASE( USER )
{
  cout << "=== USER ===" << endl;
  
  list<string> args;
  BOOST_CHECK(Parser::parse("USER username servername localhost :RealName", &args) == "USER");
  BOOST_CHECK(args.size() == 4);
  BOOST_CHECK(args.front() == "username");
  args.pop_front();
  BOOST_CHECK(args.front() == "servername");
  args.pop_front();
  BOOST_CHECK(args.front() == "localhost");
  args.pop_front();
  BOOST_CHECK(args.front() == ":RealName");
  
}
