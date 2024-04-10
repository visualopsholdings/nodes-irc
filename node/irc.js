var irc = require('./modules/irc/index'),
	db = require('./modules/db/index');
	
var port = parseInt(process.argv[2]);

// initialise the db
db.init('mongodb://fiveEstellas:visualops@localhost/fiveEstellas');

const server = irc.createServer();
server.listen(6667);
irc.initWorker(server, port + 12, port + 13);
