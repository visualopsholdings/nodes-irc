const {
  ERR_NONICKNAMEGIVEN,
  ERR_NICKNAMEINUSE
} = require('../replies')
const {debuglog} = require('util');
const debug = debuglog('ircs:commands:nick')

module.exports = function nick ({ user, server, parameters: [ nickname ] }) {
  nickname = nickname.trim()

  debug('NICK', user.mask(), nickname)

  if (!nickname || nickname.length === 0) {
    return user.send(server, ERR_NONICKNAMEGIVEN, [ 'No nickname given' ]);
  }

  if (nickname === user.nickname) {
    // ignore
    return
  }

  const lnick = nickname.toLowerCase()
  
  // remove the temporary user.
  for (var i=0; i<server.users.length; i++) {
    const user = server.users[i];
    if (user.nickname == lnick && !user.socket) {
      server.users.splice(i, 1);
      break;
    }
  }
  
  if (server.users.some((us) => us.nickname &&
                                us.nickname.toLowerCase() === lnick &&
                                us !== user)) {
    return user.send(server, ERR_NICKNAMEINUSE, 
      [ user.nickname, nickname, ':Nickname is already in use' ])
  }
  user.nickname = nickname;
  user.send(user, 'NICK', [ nickname ])
  user.channels.forEach(chan => chan.broadcast(user, 'NICK', [ nickname ]));
}
