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

## Current development focus

### Implement in C++

## License

ZMQIRC is licensed under [version 3 of the GNU General Public License] contained in LICENSE.

## Change Log

### 10 Apr 2024
- Initial project creation.

### 15 Apr 2024
- Cucumber tests, slight rename to include the ZMQChat project (new) which is an open source
implementation of the proprietary "Visual Ops" server.

