var zmq = require('zeromq');

var port = parseInt(process.argv[2]);

var pubport = port + 12;
var repport = port + 13;

var pubSocket = zmq.socket('pub');	
pubSocket.bindSync('tcp://*:' + pubport);
console.log('Local PUB bound to port ' + pubport);

var repSocket = zmq.socket('rep');	
repSocket.bindSync('tcp://127.0.0.1:' + repport);
console.log('Local REP bound to port ' + repport);

//var logger = { log: function() {} };
var logger = console;

var self = this;
repSocket.on('message', (msg) => {

  var m = JSON.parse(msg);
  logger.log(m);
  
  if (m.type == "login") {
    if (m.username == "tracy") {
      repSocket.send(JSON.stringify({ type: "user", id: "u1", name: m.username, 
        streams: [
          { name: "My Conversation", id: "s1", policy: "p1" }
        ]
      }));
      return;
    }
    if (m.username == "leanne") {
      repSocket.send(JSON.stringify({ type: "user", id: "u2", name: m.username, 
        streams: [
          { name: "My Conversation", id: "s1", policy: "p1" }
        ]
      }));
      return;
    }
    repSocket.send(JSON.stringify({ type: "err", msg: "User not found" }));
    return;
  }
  if (m.type == "message") {
    if (m.user != "u1" && m.user != "u2") {
      repSocket.send(JSON.stringify({ type: "err", msg: "not correct user" }));
      return;
    }
    if (m.stream != "s1") {
      repSocket.send(JSON.stringify({ type: "err", msg: "not correct stream" }));
      return;
    }
    if (m.policy != "p1") {
      repSocket.send(JSON.stringify({ type: "err", msg: "not correct policy" }));
      return;
    }
    if (m.text == "hello") {
      repSocket.send(JSON.stringify({ type: "message", text: "world", stream: "s1", policy: "p1", user: m.user == "u1" ? "u2" : "u1"}));
      return;
    }
    console.log("got", m.text, "from", m.user);
    repSocket.send(JSON.stringify({ type: "ack" }));
    return;
  }
  if (m.type == "policyusers") {
    if (m.policy != "p1") {
      repSocket.send(JSON.stringify({ type: "err", msg: "not correct policy" }));
      return;
    }
    repSocket.send(JSON.stringify({ type: "policyusers", id: "p1", 
      users: [
        { id: "u1", name: "tracy", fullname: "Tracy" },
        { id: "u2", name: "leanne", fullname: "Leanne" }
      ]
    }));
    return;
  }
  
  logger.log("REQ got", m);
  repSocket.send(JSON.stringify({ type: "err", msg: "not implemented" }));

});
