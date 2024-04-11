# ZMQIRC

A frontend for IRC that uses ZMQ to send/receive messages.

## Visual Ops

Visual Ops is a messaging system (and more) that exists on the edge. Visual Ops nodes are
standalone servers which can be formed into a tree of servers delivering new messages to users
using ZMQ message queues and (in the web) Web sockets.

- There is a full featured Web App which can be configured as a standalone app on iOS and Android
and any device that can run a browser.
- Messages persist in a MongoDB database.
- A security system allows subsets of users to View and Edit "Streams" which are analagous to Channels
in IRC.
- A system called "Nodes" correctly delivers missing messages when nodes "go away".
- The system useds ZMQ message queues in a novel fashion which are the backbone for delivery
and use EC (Eliptical Curve) Cryptography to ensure transport security.
- Visual Ops is monolithic. Every server is completely standalone and require no more resources
from the internet (Cloud).

For these reasons, it would be the perfect backend for IRC and solve many of the existing drawbacks
and problems that plague IRC

This project will implement an IRC front end in C++ that uses ZMQ to talk to the Visual Ops "Nodes"
system for message delivery, security, persistence etc.

## Reference version using nodejs

A reference version of what we are aiming for exists in "node".

To use it with Visual Ops:

```
$ git clone https://github.com/visualopsholdings/zmqirc.git
$ cd zmqirc/node
$ npm install
$ forever start --killSignal=SIGINT irc.js 3000
```

On reboot, Visual ops will look for the irc.js file and correctly start it up so it really
just needs to exist in "zmqirc/node".

This is a modified version of:

https://github.com/song940/node-irc

## Protocol

The protocol used to talk to Nodes is sending stringified JSON over 2 ZMQ message queues.

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

Since the Visual Ops (and the node system) is persistent, when you join a stream, you can query the 
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

The end to end tests use cucumber (Ruby) to get that going.

```
$ bundle install
$ bundle exec cucumber
```

## Current development focus

### Cucumber testing framework

## License

ZMQIRC is licensed under [version 3 of the GNU General Public License] contained in LICENSE.

## Change Log

### 10 Apr 2024
- Initial project creation.

