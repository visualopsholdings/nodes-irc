var irc = require('./node-irc/index'),
    zmq = require('zeromq'),
    pkg = require('./package.json');

var port = parseInt(process.argv[2]);

const server = irc.createServer();
server.listen(6667);

var subport = port + 12;
var reqport = port + 13;

var subSocket = zmq.socket('sub');	
subSocket.connect('tcp://127.0.0.1:' + subport);
subSocket.subscribe('');
console.log('Local SUB connected to port ' + subport);

server.reqSocket = zmq.socket('req');	
server.reqSocket.connect('tcp://127.0.0.1:' + reqport);
console.log('Local REQ connected to port ' + reqport);

var logger = { log: function() {} };
//  var logger = console;

var self = this;
subSocket.on('message', (msg) => {

  var m = JSON.parse(msg);
  logger.log("got", m);
  var user = server.findUserId(m.user);
  if (!user) {
    console.log("ignoring user", m.user);
  }
  else {
    var target = server.findChannelId(m.stream);
    if (target) {
      target.send(user, 'PRIVMSG', [ target.name, `:${m.text}` ]);
    }
  }
});

server.reqSocket.on('message', (msg) => {

  var m = JSON.parse(msg);
  logger.log("got reply", m);
  if (m.type == "ack") {
    logger.log("got ack");
  }
  else if (m.type == "user") {
    var user = server.findUserUsername(m.name);
    if (!user) {
      console.log("unknown user", m.name);
      return;
    }
    user.id = m.id;
    user.send(server, '001', [ user.nickname, ':Welcome' ]);
    user.send(server, '002', [ user.nickname, `:Your host is ${server.hostname} running version ${pkg.version}` ]);
    user.send(server, '003', [ user.nickname, `:This server was created ${server.created}` ]);
    user.send(server, '004', [ user.nickname, pkg.name, pkg.version ]);
    user.send(server, 'MODE', [ user.nickname, '+w' ]);
    m.streams.forEach(s => {
      var channelName = "#" + s.name.replace(/ /g, '+').toLowerCase();
      server.createChannel(channelName, s.id, s.policy);
    });
  }
  else if (m.type == "policyusers") {
    var channel = server.findChannelPolicy(m.id);
    if (!channel) {
      console.log("unknown channel policy", m.id);
      return;
    }
    m.users.forEach(u => {
      var user = server.findUserId(u.id);
      if (!user) {
        user = server.addOtherUser(u.id, u.name, u.fullname);
        channel.join(user);
        channel.send(user, 'JOIN', [ channel.name, user.username, `:${user.realname}` ])
      }
    });
  }
  else if (m.type == "message") {
    var target = server.findChannelId(m.stream);
    if (target) {
      var user = server.findUserId(m.user);
      if (user) {
        target.send(user, 'PRIVMSG', [ target.name, `:${m.text}` ]);
      }
      else {
        console.log("unknown user", m.user);
      }
    }
    else {
      console.log("unknown stream", m.stream);
    }
  }
  else if (m.type == "err") {
    console.log("got err", m.msg);
  }
  else {
    console.log("unknown msg type", m.type);
  }
  
});

//irc.initWorker(server, port + 12, port + 13);
