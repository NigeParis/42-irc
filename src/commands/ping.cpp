#include "../../include/Server.hpp"


void Server::handlePing(int client_fd, const Command &command) {
  Client *client = clients[client_fd];
  std::string response = buildMessage("ircserv", "PONG", 
                                       std::vector<std::string>(1, client->nickname), 
                                       command.trailing);
  sendResponse(client_fd, response);
}