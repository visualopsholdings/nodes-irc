# nodes-irc

A frontend for IRC that uses ZMQ to send/receive messages.

## Visual Ops

This is a companion project to:

https://github.com/visualopsholdings/nodes

Visual Ops is a proprietary (closed source) system that runs on very low end hardware
like T4G's in AWS, or on a Raspberry Pi 4/5.

This "plugin" can run on a Visual Ops server and provides simple IRC access to Visual Ops 
allowing for any IRC client or IRC server to talk to Visual Ops via IRC.

An Open Source version of Visual Ops called "Nodes" is being developed, but for now at least
this small part of the server, the IRC protocol is available to build and use.

If you would like access to the Visual Ops software it is available as an image download 
after we get to know you.

Visit https://info.visualops.com/ for more info.

All the places that mention "Visual Ops", could be interchanged with "Nodes".

### ZMQ API

This project communicate with Nodes using the ZMQ message queues.

A PUB/SUB socket on port tcp://127.0.0.1:3012.

This is used to receive new messages from other nodes and users locally connected to this node.

A REP/REQ socket on port tcp://127.0.0.1:3013.

This is used to send messages to other nodes and users connected locally to this node. 
This port is also used to "ask" for things of the Nodes system such as login etc.

[Here are the formats for the messages](https://github.com/visualopsholdings/nodes/blob/main/src/msg/README.md)

### Supported IRC commands

#### PASS [password]

Sends the "login" command below, you can pass your VID here which is all that is needed by 
Visual Ops to return your nickname and your real name.

#### LIST

List all channels (streams in Visual Ops)

#### JOIN

Join a particular channel.

#### QUIT

Log off the server.

#### PRIVMSG

Send a message to the server.

## Building

The development process for all of this code used a normal Linux environment with the BOOST
libraries and a C++ compiler.

So on your Linux (or mac using Homebrew etc), get all you need: (These steps have been tested
on Ubuntu 24.04 on a Pi5 and in a TG4 in AWS):

```
sudo apt-get update
sudo apt-get -y install g++ gcc make cmake
```

### Prerequisites

We rely on https://github.com/visualopsholdings/nodes being built first, so do that, and then
in your shell, define a variable to point to that:

```
export NODES_HOME=/where/nodes/went
```

For convenience, do all these inside a folder you can remove, they take a lot of disk space
but aren't used once instsalled.

```
mkdir working
cd working
```

#### Boost 1.85.0

if you can get this or later with a simple tool then do that, otherwise you will
need to build it from source:

[Instructions](https://github.com/visualopsholdings/nodes-devops/blob/main/dev/BOOST.md)

#### ZMQ

If you can get zeromq 4.3.5 or later, then use that otherwise you will need to build it
all from source.

[Instructions](https://github.com/visualopsholdings/nodes-devops/blob/main/dev/ZMQ.md)

### This project

```
git clone https://github.com/visualopsholdings/nodes-irc.git
nodes-irc/scripts/build.sh
```

To run all the unit tests. And show failures.

```
cd nodes-irc/build
make && make test || cat Testing/Temporary/LastTest.log 
```

## Testing

The end to end tests use cucumber (Ruby), to get that going:

```
./test.sh
```

This will run up a dummy "Nodes" with some dummy data and run end to end tests
against the IRC client.

You can find the actual tests in "features".

To run a single test:

```
bundle exec cucumber -n "More than one user can connect and login"
```

## Installation in Visual Ops

You use the same SSL certs that visualops has, which are in /etc/letsencrypt/live/mydomain, so 
edit nodes-irc/scripts/start.sh and set it to that folder.

Once built, to start it on a Visual Ops server:

```
$ nodes-irc/scripts/start.sh
```

To stop it:

```
$ nodes-irc/scripts/stop.sh
```

On reboot, Visual ops will look for the start script file and run it up.

Make sure that port 6667 is punched through your firewall and you can connect to the Visual Ops
server and chat that way.

## Companion projects

### DevOps

https://github.com/visualopsholdings/nodes-devops

### The main nodes system

https://github.com/visualopsholdings/nodes

### A web front end

https://github.com/visualopsholdings/nodes-web

## Current development focus

### Add commands.

## License

Nodes is licensed under [version 3 of the GNU General Public License] contained in LICENSE.

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

## Change Log

### 10 Apr 2024
- Initial project creation.

### 15 Apr 2024
- Cucumber tests, slight rename to include the Nodes project (new) which is an open source
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

### 8 May 2024
- FInish initial C++ implementation running in Visual Ops

### 10 May 2024
- Rewrite nodes test program in C++. Get rid of nodejs.

### 12 May
- SSL support

### 16 May
- Solve "LIST" deadlock problem in SSL.

### 29 Jul 2024
- rename to https://github.com/visualopsholdings/nodes-irc
- implement end to end tests using the real nodes.

