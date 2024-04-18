#   CMakeLists.txt
#
#   Author: Paul Hamilton (paul@visualops.com)
#   Date: 16-Apr-2024
#
#   Tests
#
#   Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
#   
#   https://github.com/visualopsholdings/zmqirc

cmake_minimum_required(VERSION 3.5)

project (zmqirctest)
  find_package(Boost COMPONENTS unit_test_framework filesystem REQUIRED)
  add_definitions(-DBOOST_ALL_DYN_LINK) 
  enable_testing()

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

if (APPLE)
  include_directories(/usr/local/include)
endif ()

add_executable(TestSession testsession.cpp)
  target_link_libraries(TestSession ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(TestSession TestSession)
