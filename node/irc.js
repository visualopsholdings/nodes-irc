var irc = require('./node-irc/index');
	
var port = parseInt(process.argv[2]);

const server = irc.createServer();
server.listen(6667);
irc.initWorker(server, port + 12, port + 13);
