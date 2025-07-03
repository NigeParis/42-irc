/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   notice.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 14:32:26 by nrobinso          #+#    #+#             */
/*   Updated: 2025/07/03 14:32:29 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::handleNotice(int client_fd, const Command &command) {
  if (clients.find(client_fd) == clients.end()) {
    return;
  }

  if (!clients[client_fd]->is_authenticated)	return;  


  if (command.params.empty()) {
    return;
  }

  if (command.trailing.empty()) {
    return;
  }

  std::string target = command.params[0];
  

  if (target[0] == '#') {
    handleChannelNotice(client_fd, target, command.trailing);
  } else {
    handlePrivateNotice(client_fd, target, command.trailing);
  }
}

void Server::handleChannelNotice(int client_fd, const std::string &target, const std::string &message) {
  Client *client = clients[client_fd];

  if (channels.find(target) == channels.end()) {
    return;
  }

  Channel *channel = channels[target];
  
  if (!channel->hasUser(client_fd)) {
    return;
  }

  std::vector<int> channel_users = channel->getClientFds();
  std::vector<std::string> privmsg_params;
  privmsg_params.push_back(target);
  std::string message_prefix = client->nickname + "!" + client->username + "@" + client->hostname;
  std::string privmsg_message = buildMessage(message_prefix, "NOTICE", privmsg_params, message);
  
  std::vector<int> recipients;
  for (size_t i = 0; i < channel_users.size(); ++i) {
    if (channel_users[i] != client_fd) {
      recipients.push_back(channel_users[i]);
    }
  }
  broadcastResponse(privmsg_message, recipients);
}

void Server::handlePrivateNotice(int client_fd, const std::string &target, const std::string &message) {
  Client *client = clients[client_fd];

  int target_fd = findClientByNickname(target);
  std::cout << "target_fd: " << target_fd << std::endl;

  if (target_fd == -1) {
    return;
  }

  std::vector<std::string> privmsg_params;
  privmsg_params.push_back(target);
  std::string message_prefix = client->nickname + "!" + client->username + "@" + client->hostname;
  std::string privmsg_message = buildMessage(message_prefix, "NOTICE", privmsg_params, message);
  sendResponse(target_fd, privmsg_message);
}



