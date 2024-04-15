const {
  ERR_NOSUCHNICK
} = require('../replies')

module.exports = function privmsg ({ user, server, parameters: [ targetName, content ] }) {
  let target
  if (targetName[0] === '#' || targetName[0] === '&') {
    target = server.findChannel(targetName)
    if (target) {
      server.newMessage({ user: user.id, text: content, stream: target.id, policy: target.policy });
//      target.broadcast(user, 'PRIVMSG', [ target.name, `:${content}` ]);
    }
  }/* else {
    target = server.findUser(targetName)
    if (target) {
      console.log("would send");
      target.send(user, 'PRIVMSG', [ target.nickname, `:${content}` ]);
    }
  }*/

  if (!target) {
    user.send(server, ERR_NOSUCHNICK, [ user.nickname, targetName, ':No such nick/channel' ])
  }
}
