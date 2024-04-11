var irc = require('./node-irc/index'),
    zmq = require('zeromq'),
    pkg = require('./package.json');

var port = parseInt(process.argv[2]);

const server = irc.createServer();
server.listen(6667);

var subport = port + 12;
var reqport = port + 13;

var pullSocket = zmq.socket('sub');	
pullSocket.connect('tcp://127.0.0.1:' + subport);
pullSocket.subscribe('');
console.log('Local PUB/SUB connected to port ' + subport);

server.localSocket = zmq.socket('req');	
server.localSocket.connect('tcp://127.0.0.1:' + reqport);
console.log('Local REP/REQ connected to port ' + reqport);

var logger = { log: function() {} };
//  var logger = console;

var self = this;
pullSocket.on('message', (msg) => {

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

server.localSocket.on('message', (msg) => {

  var m = JSON.parse(msg);
  logger.log("got reply", m);
  if (m.type == "ack") {
    logger.log("got ack");
  }
  else if (m.type == "user") {
    var user = server.findUserUsername(m.name);
    if (user) {
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
    else {
      console.log("unknown user", m.name);
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
