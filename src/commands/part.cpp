#include "../../include/Server.hpp"

void Server::handlePart(int client_fd, const Command &command) {
  if (clients.find(client_fd) == clients.end()) {
    return;
  }

  if (!clients[client_fd]->is_authenticated) {
    std::string response = buildMessage("ircserv", "451", 
                                       std::vector<std::string>(1, clients[client_fd]->nickname), 
                                       "You have not registered");
    sendResponse(client_fd, response);
    return;
  }

  Client *client = clients[client_fd];

  if (command.params.empty()) {
    std::string response = buildMessage("ircserv", "461", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "Not enough parameters");
    sendResponse(client_fd, response);
    return;
  }

  std::string reason = command.trailing.empty() ? client->nickname : command.trailing;
  std::string channel_name = command.params[0];
  
  if (channels.find(channel_name) == channels.end()) {
    std::vector<std::string> params;
    params.push_back(client->nickname);
    params.push_back(channel_name);
    std::string response = buildMessage("ircserv", "403", params, "No such channel");
    sendResponse(client_fd, response);
    return;
  }

  Channel *channel = channels[channel_name];

  if (!channel->hasUser(client_fd)) {
    std::vector<std::string> params;
    params.push_back(client->nickname);
    params.push_back(channel_name);
    std::string response = buildMessage("ircserv", "442", params, "You're not on that channel");
    sendResponse(client_fd, response);
    return;
  }

  std::string part_prefix = client->nickname + "!" + client->username + "@" + client->hostname;
  std::vector<std::string> part_params;
  part_params.push_back(channel_name);
  std::string part_message = buildMessage(part_prefix, "PART", part_params, reason);

  std::vector<int> channel_users = channel->getClientFds();

  broadcastResponse(part_message, channel_users);

  channel->removeUser(client_fd);

  if (channel->getClientFds().empty()) {
    delete channel;
    channels.erase(channel_name);
  }
}
