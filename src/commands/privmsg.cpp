/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 14:33:01 by nrobinso          #+#    #+#             */
/*   Updated: 2025/07/03 14:33:03 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::handlePrivmsg(int client_fd, const Command &command) {
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
    std::string response = buildMessage("ircserv", "411", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "No recipient given (PRIVMSG)");
    sendResponse(client_fd, response);
    return;
  }

  if (command.trailing.empty()) {
    std::string response = buildMessage("ircserv", "412", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "No text to send");
    sendResponse(client_fd, response);
    return;
  }

  std::string target = command.params[0];
  

  if (target[0] == '#') {
    handleChannelMessage(client_fd, target, command.trailing);
  } else {
    handlePrivateMessage(client_fd, target, command.trailing);
  }
}

void Server::handleChannelMessage(int client_fd, const std::string &target, const std::string &message) {
  Client *client = clients[client_fd];

  if (channels.find(target) == channels.end()) {
    std::vector<std::string> params;
    params.push_back(client->nickname);
    params.push_back(target);
    std::string response = buildMessage("ircserv", "404", params, "Cannot send to channel");
    sendResponse(client_fd, response);
    return;
  }

  Channel *channel = channels[target];
  
  if (!channel->hasUser(client_fd)) {
    std::vector<std::string> params;
    params.push_back(client->nickname);
    params.push_back(target);
    std::string response = buildMessage("ircserv", "404", params, "Cannot send to channel");
    sendResponse(client_fd, response);
    return;
  }

  std::vector<int> channel_users = channel->getClientFds();
  std::vector<std::string> privmsg_params;
  privmsg_params.push_back(target);
  std::string message_prefix = client->nickname + "!" + client->username + "@" + client->hostname;
  std::string privmsg_message = buildMessage(message_prefix, "PRIVMSG", privmsg_params, message);
  
  std::vector<int> recipients;
  for (size_t i = 0; i < channel_users.size(); ++i) {
    if (channel_users[i] != client_fd) {
      recipients.push_back(channel_users[i]);
    }
  }
  broadcastResponse(privmsg_message, recipients);
}

void Server::handlePrivateMessage(int client_fd, const std::string &target, const std::string &message) {
  Client *client = clients[client_fd];

  int target_fd = findClientByNickname(target);
  std::cout << "target_fd: " << target_fd << std::endl;

  if (target_fd == -1) {
    std::vector<std::string> params;
    params.push_back(client->nickname);
    params.push_back(target);
    std::string response = buildMessage("ircserv", "401", params, "No such nick/channel");
    sendResponse(client_fd, response);
    return;
  }

  std::vector<std::string> privmsg_params;
  privmsg_params.push_back(target);
  std::string message_prefix = client->nickname + "!" + client->username + "@" + client->hostname;
  std::string privmsg_message = buildMessage(message_prefix, "PRIVMSG", privmsg_params, message);
  sendResponse(target_fd, privmsg_message);
}


