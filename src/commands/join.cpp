#include "../../include/Server.hpp"

void Server::handleJoin(int client_fd, const Command &command) {
  if (clients.find(client_fd) == clients.end()) {
    return;
  }

  if (!clients[client_fd]->is_authenticated) {
    Command cmd;
    cmd.prefix = "ircserv";
    cmd.name = "451";
    cmd.params.push_back(clients[client_fd]->nickname);
    cmd.trailing = "You have not registered";
    std::string response = buildMessage(cmd.prefix, cmd.name, cmd.params, cmd.trailing);
    sendResponse(client_fd, response);
    return;
  }

  if (command.params.empty()) {
    Command cmd;
    cmd.prefix = "ircserv";
    cmd.name = "461";
    cmd.params.push_back(clients[client_fd]->nickname);
    cmd.trailing = "Not enough parameters";
    std::string response = buildMessage(cmd.prefix, cmd.name, cmd.params, cmd.trailing);
    sendResponse(client_fd, response);
    return;
  }

  if (command.params.size() < 1 || command.params[0].empty()) {
    Command cmd;
    cmd.prefix = "ircserv";
    cmd.name = "461";
    cmd.params.push_back(clients[client_fd]->nickname);
    cmd.trailing = "Not enough parameters";
    std::string response = buildMessage(cmd.prefix, cmd.name, cmd.params, cmd.trailing);
    sendResponse(client_fd, response);
    return;
  }

  if (command.params[0][0] != '#') {
    Command cmd;
    cmd.prefix = "ircserv";
    cmd.name = "476";
    cmd.params.push_back(clients[client_fd]->nickname);
    cmd.trailing = "Channel name must begin with '#'";
    std::string response = buildMessage(cmd.prefix, cmd.name, cmd.params, cmd.trailing);
    sendResponse(client_fd, response);
    return;
  }

  if (channels.find(command.params[0]) == channels.end()) {
    channels[command.params[0]] = new Channel(command.params[0]);
  }

  Channel *channel = channels[command.params[0]];

  if (!channel->isWhiteListed(client_fd)) {
    Command cmd;
    cmd.prefix = "ircserv";
    cmd.name = "473";
    cmd.params.push_back(clients[client_fd]->nickname);
    cmd.trailing = "Cannot join channel (not whitelisted)";
    std::string response = buildMessage(cmd.prefix, cmd.name, cmd.params, cmd.trailing);
    sendResponse(client_fd, response);
    return;
  }

  if (channel->isPrivate) {
    if (command.params.size() < 2 ||
        !channel->verifyPassword(command.params[1])) {
      Command cmd;
      cmd.prefix = "ircserv";
      cmd.name = "461";
      cmd.params.push_back(clients[client_fd]->nickname);
      cmd.trailing = "Not enough parameters";
      std::string response = buildMessage(cmd.prefix, cmd.name, cmd.params, cmd.trailing);
      sendResponse(client_fd, response);
      return;
    }
  }

  Client *client = clients[client_fd];
  (void)*client;
}
