#include "../../include/Server.hpp"

void Server::handlePass(int client_fd, const Command &command) {

  Client *client = clients[client_fd];
  Command cmd;
  cmd.prefix = "ircserv";
  cmd.params.push_back(client->nickname);

  if (client->is_authenticated) {

    cmd.name = "462";
    cmd.trailing = "You may not reregister";
    std::string response = buildMessage(cmd);
    sendResponse(client_fd, response);
    disconnectClient(client_fd);
    return;
  }

  if (command.params.size() < 1) {
    cmd.name = "461";
    cmd.trailing = "Not enough parameters";
    std::string response = buildMessage(cmd);
    sendResponse(client_fd, response);
    return;
  }

  if (command.params[0] != password_) {
    cmd.name = "464";
    cmd.trailing = "Password incorrect";
    std::string response = buildMessage(cmd);
    sendResponse(client_fd, response);
    disconnectClient(client_fd);
    return;
  }

  client->is_authenticated = true;
  return;
}
