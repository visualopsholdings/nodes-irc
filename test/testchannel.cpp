/*
  testchannel.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Apr-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include <iostream>

#include "parser.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( normalise )
{
  cout << "=== normalise ===" << endl;
  
  BOOST_CHECK(Parser::normalise("a") == "a");
  BOOST_CHECK(Parser::normalise("A") == "a");

}

BOOST_AUTO_TEST_CASE( streamNames )
{
  cout << "=== streamNames ===" << endl;
  
  BOOST_CHECK(Parser::from_stream_name("A Stream Name") == "#a+stream+name");

}
