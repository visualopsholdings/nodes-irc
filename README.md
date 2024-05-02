# ZMQIRC

A frontend for IRC that uses ZMQ to send/receive messages.

## Visual Ops

This is a companion project to:

https://github.com/visualopsholdings/zmqchat

## Reference version using nodejs

A reference version of what we are aiming for exists in "nodejs".

To use it with Visual Ops:

```
$ git clone https://github.com/visualopsholdings/zmqirc.git
$ cd zmqirc/nodejs
$ npm install
$ forever start --killSignal=SIGINT irc.js 3000
```

On reboot, Visual ops will look for the irc.js file and correctly start it up so it really
just needs to exist in "zmqirc/nodejs".

Make sure that port 6667 is punched through yoru firewall and you can connect to your Visual Ops
server and chat that way.

This is a modified version of:

https://github.com/song940/node-irc

## Protocol

The protocol used to talk to ZMQChat is sending stringified JSON over 2 ZMQ message queues.

### ZMQ sockets

A SUB socket on port tcp://127.0.0.1:3012 which is used to receive new messages from other nodes
and users locally connected to this node.

A REQ socket on port tcp://127.0.0.1:3013 which is used to send messages to other nodes and
users connected locally to this node. This port is also used to "ask" for things of the Visual Ops
system such as login etc.

### JSON formats

#### Login

```
{ 
  "type": "login", 
  "username": "username" 
}
```

This will be processed by the Visual Ops system and it will return:

```
{ 
  "type": "user", 
  "name": "tracy",
  "id": "user guid",
  streams: [
    {
      "name": "Conversation 1",
      "stream": "stream guid",
      "policy": "stream policy guid"
    }
  ]
}
```

#### Policy users

Since the Visual Ops (and the ZMQChat system) is persistent, when you join a stream, you can query the 
users in that stream by the stream policy and join those users too.

```
{ 
  "type": "policyusers", 
  "polict": "stream policy guid" 
}
```

This will be processed by the Visual Ops system and it will return:

```
{ 
  "type": "user", 
  "name": "policyusers",
  "id": "stream policy guid",
  users: [
    {
      "name": "tracy",
      "fullname": "Tracy",
      "id": "user guid"
    },
    {
      "name": "leanne",
      "fullname": "Leanne",
      "id": "user guid"
    }
  ]
}
```

#### Message

This is sent to the REQ socket when a message happens on IRC, and will be received on the 
SUB socket when a new message is received.

```
{ 
  "type": "message", 
  "text": "Message text",
  "stream": "stream guid",
  "policy": "stream policy guid",
  "user": "user guid"
}
```

#### Acknowlege

This is received on the REQ socket when a Message is sent.

```
{ 
  "type": "ack"
}
```

#### Error

This is received on the REQ socket when a a bad request has happened

```
{ 
  "type": "err",
  "msg": "LOL what are you doing!"
}
```

## Testing

The end to end tests use cucumber (Ruby), to get that going:

```
$ bundle install
$ bundle exec cucumber
```

This will run up a dummy "ZMQChat" with some dummy data and run end to end tests
against the IRC client.

You can find the actual tests in "features".

To run a single test:

```
bundle exec cucumber -n "More than one user can connect and login"
```

To turn on logging for the old NodeJS while you run the tests:

```
export NODE_DEBUG=ircs:*
```

## C++ Development

The development process for all of this code used a normal Linux environment with the BOOST
libraries and a C++ compiler.

So on your Linux (or mac using Homebrew etc), get all you need:

```
sudo apt-get update
sudo apt-get -y install g++ gcc make cmake
```

The build the prequesites we need:

For convenience.

```
mkdir working
cd working
```

Boost 1.85.0 if you can get this or later with a simple tool then do that, otherwise you will
need to build it from source:

```
wget https://github.com/boostorg/boost/releases/download/boost-1.85.0/boost-1.85.0-b2-nodocs.tar.gz
tar xzf boost-1.85.0-b2-nodocs.tar.gz 
cd boost-1.85.0
./bootstrap.sh --prefix=/usr --with-python=python3
./b2 stage threading=multi link=shared boost.stacktrace.from_exception=off

as root user:
./b2 install threading=multi link=shared
```

ZMQ Stuff.

```
git clone https://github.com/zeromq/libzmq.git
cd libzmq
mkdir build
cd build
cmake ..
make
sudo make install
cd ../..

git clone https://github.com/zeromq/cppzmq
cd cppzmq
mkdir build
cd build
cmake ..
make
sudo make install
cd ../..

git clone https://github.com/nlohmann/json
cd json
mkdir build
cd build
cmake ..
make
sudo make install
cd ../..
```

Now this project:

```
git clone https://github.com/visualopsholdings/zmqirc.git
cd zmqirc
mkdir build
cd build
cmake ..
make
make test
```

To run all the tests. And show failures.

```
make && make test || cat /Users/paul/Dev/zmqirc/test/build/Testing/Temporary/LastTest.log
```

## Current development focus

### Get C++ working for all end to end tests.

## License

ZMQIRC is licensed under [version 3 of the GNU General Public License] contained in LICENSE.

## Acknowlegements

### C++ and Boost inspiration

- https://github.com/skywodd/Boost_IRC_server

### NodeJS IRC inspiration

- https://github.com/song940/node-irc

### ZeroMQ messaging

- https://zeromq.org/

### Boost C++ library

- https://www.boost.org/

### End to end testing

- https://cucumber.io/


### JSON for C++

- https://github.com/nlohmann/json

## Change Log

### 10 Apr 2024
- Initial project creation.

### 15 Apr 2024
- Cucumber tests, slight rename to include the ZMQChat project (new) which is an open source
implementation of the proprietary "Visual Ops" server.

### 18 Apr 2024
- Implement server and session.

### 22 Apr 2024
- Simple parser, handle NICK

### 24 Apr 2024
- command list, handle USER and send ZMQ login to record user.

### 26 Apr 2024
- start to implement channels mapped from streams.

### 29 Apr 2024
- start to implemement users.

### 30 Apr 2024
- finish off other users in the stream.
- finish off PRIVMSG

### 1 May 2024
- Get ZMQ working in background thread to handle messages correctly.
