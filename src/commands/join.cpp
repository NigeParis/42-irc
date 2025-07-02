#include "../../include/Server.hpp"

void Server::handleJoin(int client_fd, const Command &command) {
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

  std::vector<std::string> params;
  params.push_back(clients[client_fd]->nickname);

  if (command.params.empty() || command.params.size() < 1 ||  command.params[0].empty()) {
    std::string response = buildMessage("ircserv", "461", 
                                       std::vector<std::string>(1, clients[client_fd]->nickname), 
                                       "Not enough parameters");
    sendResponse(client_fd, response);
    return;
  }

  if (command.params[0][0] != '#') {
    std::string response = buildMessage("ircserv", "476", 
                                       std::vector<std::string>(1, clients[client_fd]->nickname), 
                                       "Channel name must begin with '#'");
    sendResponse(client_fd, response);
    return;
  }

  if (channels.find(command.params[0]) == channels.end()) {
    channels[command.params[0]] = new Channel(command.params[0], getCurrentTime());
  }

  Channel *channel = channels[command.params[0]];

  if (!channel->isWhiteListed(client_fd)) {
    std::string response = buildMessage("ircserv", "473", 
                                       std::vector<std::string>(1, clients[client_fd]->nickname), 
                                       "Cannot join channel (not whitelisted)");
    sendResponse(client_fd, response);
    return;
  }

  if (channel->isPrivate) {
    if (command.params.size() < 2 ||
        !channel->verifyPassword(command.params[1])) {
      std::string response = buildMessage("ircserv", "461", 
                                         std::vector<std::string>(1, clients[client_fd]->nickname), 
                                         "Not enough parameters");
      sendResponse(client_fd, response);
      return;
    }
  }

  if (channel->userLimit > 0 && channel->getClientFds().size() >= static_cast<size_t>(channel->userLimit)) {
    std::string response = buildMessage("ircserv", "471", 
                                       std::vector<std::string>(1, clients[client_fd]->nickname), 
                                       "Cannot join channel (user limit reached)");
    sendResponse(client_fd, response);
    return;
  }

  Client *client = clients[client_fd];
  channel->addUser(client);
  sendWelcomeJoinMessage(client_fd, channel->name);

}

void Server::sendWelcomeJoinMessage(int client_fd, const std::string &channel_name) {

  Client *client = clients[client_fd];
  Channel *channel = channels[channel_name];

  std::string join_prefix = client->nickname + "!" + client->username + "@" + client->hostname;
  std::vector<std::string> join_params;
  join_params.push_back(channel_name);
  std::string join_message = buildMessage(join_prefix, "JOIN", join_params, "");
  
  std::vector<int> channel_users = channel->getClientFds();
  broadcastResponse(join_message, channel_users);

  if (!channel->getTopic().empty()) {
    std::vector<std::string> topic_params;
    topic_params.push_back(client->nickname);
    topic_params.push_back(channel_name);
    std::string topic_response = buildMessage("ircserv", "332", topic_params, channel->getTopic());
    sendResponse(client_fd, topic_response);
  }

  std::string names_list = "";
  for (size_t i = 0; i < channel_users.size(); ++i) {
    Client *user = clients[channel_users[i]];
    if (!names_list.empty()) {
      names_list += " ";
    }
    if (channel->isOperator(channel_users[i])) {
      names_list += "@";
    }
    names_list += user->nickname;
  }

  std::vector<std::string> namreply_params;
  namreply_params.push_back(client->nickname);
  namreply_params.push_back("="); 
  namreply_params.push_back(channel_name);
  std::string namreply_response = buildMessage("ircserv", "353", namreply_params, names_list);
  sendResponse(client_fd, namreply_response);

  std::vector<std::string> endnames_params;
  endnames_params.push_back(client->nickname);
  endnames_params.push_back(channel_name);
  std::string endnames_response = buildMessage("ircserv", "366", endnames_params, "End of /NAMES list");
  sendResponse(client_fd, endnames_response);
}

