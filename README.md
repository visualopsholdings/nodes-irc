# ZMQIRC

A frontend for IRC that uses ZMQ to send/receive messages.

## Visual Ops

Visual Ops is a messaging system (and more) that exists on the edge. Visual Ops nodes are
standalone servers which can be formed into tree of servers delivering new messages to users
using ZMQ message queues and (in the web) Web sockets.

- There is a full featured Web App which can be configured as a standalone app on iOS and Android
and any device that can run a browser.
- Messages persist in a MongoDB database.
- A security system allows subsets of users to View and Edit "Streams" which are analagous to Channels
in IRC.
- A system called "Nodes" correctly delivers missing messages when nodes "go away".
- The system useds ZMQ message queues in a novel fashion which are the backbone for delivery
and use EC (Eliptical Curve) Cryptography to ensure transport security.
- Visual Ops is monolithic. Every server is completely standalone and required no more resources
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


## Current development focus

### NodeJS reference implementation
  
## Change Log

### 10 Apr 2024
- Initial project creation.

