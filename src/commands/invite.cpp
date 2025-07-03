/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 14:31:35 by nrobinso          #+#    #+#             */
/*   Updated: 2025/07/03 14:31:38 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::handleInvite(int client_fd, const Command &command) {
  if (clients.find(client_fd) == clients.end()) {
    return;
  }

  Client *client = clients[client_fd];

  if (!client->is_authenticated) {
    std::string response = buildMessage("ircserv", "451", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "You have not registered");
    sendResponse(client_fd, response);
    return;
  }

  if (command.params.empty()) {
    listInvites(client_fd);
    return;
  }

  if (command.params.size() < 2) {
    std::string response = buildMessage("ircserv", "461", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "Not enough parameters");
    sendResponse(client_fd, response);
    return;
  }

  sendInvite(client_fd, command.params[0], command.params[1]);
}

void Server::listInvites(int client_fd) {
  Client *client = clients[client_fd];

  for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it) {
    Channel *channel = it->second;
    
    if (channel->isInvited(client_fd)) {
      std::vector<std::string> params;
      params.push_back(client->nickname);
      params.push_back(channel->name);
      std::string response = buildMessage("ircserv", "336", params, "");
      sendResponse(client_fd, response);
    }
  }

  std::vector<std::string> params;
  params.push_back(client->nickname);
  std::string response = buildMessage("ircserv", "337", params, "End of /INVITE list");
  sendResponse(client_fd, response);
}

void Server::sendInvite(int client_fd, const std::string &target_nickname, const std::string &channel_name) {
  Client *client = clients[client_fd];

  if (!isChannelExists(channel_name)) {
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

  if (channel->isInviteOnly && !channel->isOperator(client_fd)) {
    std::vector<std::string> params;
    params.push_back(client->nickname);
    params.push_back(channel_name);
    std::string response = buildMessage("ircserv", "482", params, "You're not channel operator");
    sendResponse(client_fd, response);
    return;
  }

  int target_fd = findClientByNickname(target_nickname);
  if (target_fd == -1) {
    std::vector<std::string> params;
    params.push_back(client->nickname);
    params.push_back(target_nickname);
    std::string response = buildMessage("ircserv", "401", params, "No such nick/channel");
    sendResponse(client_fd, response);
    return;
  }

  if (channel->hasUser(target_fd)) {
    std::vector<std::string> params;
    params.push_back(client->nickname);
    params.push_back(target_nickname);
    params.push_back(channel_name);
    std::string response = buildMessage("ircserv", "443", params, "is already on channel");
    sendResponse(client_fd, response);
    return;
  }

  channel->addUserToInvitedUsers(target_fd);

  std::vector<std::string> inviting_params;
  inviting_params.push_back(client->nickname);
  inviting_params.push_back(target_nickname);
  inviting_params.push_back(channel_name);
  std::string inviting_response = buildMessage("ircserv", "341", inviting_params, "");
  sendResponse(client_fd, inviting_response);

  std::string invite_prefix = client->nickname + "!" + client->username + "@" + client->hostname;
  std::vector<std::string> invite_params;
  invite_params.push_back(target_nickname);
  invite_params.push_back(channel_name);
  std::string invite_message = buildMessage(invite_prefix, "INVITE", invite_params, "");
  sendResponse(target_fd, invite_message);
}