#include "../../include/Server.hpp"

void Server::handlePass(int client_fd, const Command &command) {

  Client *client = clients[client_fd];
  std::vector<std::string> params;
  params.push_back(client->nickname);

  if (client->is_authenticated) {

    std::string response =
        buildMessage("ircserv", "462", params, "You may not reregister");
    sendResponse(client_fd, response);
    disconnectClient(client_fd);
    return;
  }

  if (command.params.size() < 1) {
    std::string response =
        buildMessage("ircserv", "461", params, "Not enough parameters");
    sendResponse(client_fd, response);
    return;
  }

  if (command.params[0] != password_) {
    std::string response =
        buildMessage("ircserv", "464", params, "Password incorrect");
    sendResponse(client_fd, response);
    disconnectClient(client_fd);
    return;
  }

  client->is_authenticated = true;
  return;
}
