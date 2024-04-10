# zmqirc
A frontend for IRC that uses ZMQ to send/receive messages.

A reference version of what we are aiming for exists in "node".

This is a modified version of:

https://github.com/song940/node-irc

To use it with Visual Ops:

$ git clone https://github.com/visualopsholdings/zmqirc.git
$ cd zmqirc/node
$ npm install
$ forever start --killSignal=SIGINT irc.js 3000
