const tcp = require('net');
const Parser = require('./parser');
const EventEmitter = require('events');
const to = require('flush-write-stream');
const zmq = require('zeromq');
const pkg = require('../package.json');

class IRC extends EventEmitter {
  constructor(options) {
    super();
    Object.assign(this, options);
    this.socket = new tcp.Socket();
    this.socket.pipe(Parser()).pipe(to.obj((message, enc, cb) => {
      const { prefix, command, parameters } = message;
      this.emit(command, parameters);
      cb()
    }));
  }
  connect() {
    const { host, port } = this;
    return new Promise(resolve => {
      this.socket.connect(port, host, resolve);
    });
  }
  write(command, parameters) {
    this.socket.write([command].concat(parameters).join(' ') + '\r\n');
    return this;
  }
  nick() {
    const user = this;
    return this.write('NICK', [user.nickname]);
  }
  user() {
    const user = this;
    const { username, hostname = '0', servername = '*', realname } = user;
    return this.write('USER', [username, hostname, servername, `:${realname}`]);
  }
  join(channel) {
    return this.write('JOIN', [channel]);
  }
  send(message, to) {
    return this.write('PRIVMSG', [to, `:${message}`]);
  }
  ping() {
    const now = Date.now();
    return this.write('PING', [now]);
  }
  pong(x) {
    return this.write('PING', x);
  }
  notice(message, to) {
    return this.write('NOTICE', [to, `:${message}`]);
  }
  part(channel, message) {
    return this.write('PART', [channel, message]);
  }
  quit(message) {
    return this.write('QUIT', [`:${message}`]);
  }
}

IRC.Server = require('./server');
IRC.createServer = options => {
  return new IRC.Server(options);
};

IRC._sendWithUser = function(server, stream, user, text) {

    var target = server.findChannelId(stream);
    if (target) {
      target.send(user, 'PRIVMSG', [ target.name, `:${text}` ]);
    }
    
};

IRC.initWorker = function(server, subport, reqport) {
	
  var pullSocket = zmq.socket('sub');	
  pullSocket.connect('tcp://127.0.0.1:' + subport);
  pullSocket.subscribe('');
  console.log('IRC Consumer connected to port ' + subport);

  server.localSocket = zmq.socket('req');	
  server.localSocket.connect('tcp://127.0.0.1:' + reqport);
  console.log('Local connected to port ' + reqport);

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
      self._sendWithUser(server, m.stream, user, m.text);
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
};

module.exports = IRC;
