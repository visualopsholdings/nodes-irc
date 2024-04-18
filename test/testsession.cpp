/*
  testsession.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 18-Apr-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include <iostream>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( io )
{
  cout << "=== io ===" << endl;
  
  BOOST_CHECK(true);
  
}
