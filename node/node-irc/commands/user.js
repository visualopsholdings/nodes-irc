const { debuglog } = require('util');
const { 
  ERR_NEEDMOREPARAMS 
} = require('../replies');

const debug = debuglog('ircs:commands:user')
/**
 * Command: USER
   Parameters: <username> <hostname> <servername> <realname>
 */
function USER({ user, server, parameters }) {

  if (parameters.length !== 4){
    return user.send(server, ERR_NEEDMOREPARAMS, [ 'USER', ':Not enough parameters' ]);
  }

  const [ username, hostname, servername, realname ] = parameters;
  debug('USER', user.mask(), username, hostname, servername, realname);

  user.hostname = hostname;
  user.username = username;
  user.realname = realname;
  user.servername = servername;
  server.login(username);

}

module.exports = USER;