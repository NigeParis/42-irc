/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 14:32:00 by nrobinso          #+#    #+#             */
/*   Updated: 2025/07/03 14:32:02 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::handleKick(int client_fd, const Command &command) {
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

  if (command.params.size() < 2) {
    std::string response = buildMessage("ircserv", "461", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "Not enough parameters");
    sendResponse(client_fd, response);
    return;
  }

  std::string channel_name = command.params[0];
  if (channels.find(channel_name) == channels.end()) {
    std::string response = buildMessage("ircserv", "403", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "No such channel");
    sendResponse(client_fd, response);
    return;
  }

  Channel *channel = channels[channel_name];

  if (!channel->hasUser(client_fd)) {
    std::string response = buildMessage("ircserv", "442", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "You're not on that channel");
    sendResponse(client_fd, response);
    return;
  }

  if (!channel->isOperator(client_fd)) {
    std::vector<std::string> params;
    params.push_back(client->nickname);
    params.push_back(channel_name);
    std::string response = buildMessage("ircserv", "482", params, "You're not channel operator");
    sendResponse(client_fd, response);
    return;
  }

	std::string target_nickname = command.params[1];
	int target_fd = findClientByNickname(target_nickname);

	if (target_fd == -1) {
		std::vector<std::string> params;
		params.push_back(client->nickname);
		params.push_back(target_nickname);
		std::string response = buildMessage("ircserv", "401", params, "No such nick/channel");
		sendResponse(client_fd, response);
		return;
	}

  if (!channel->hasUser(target_fd)) {
    std::vector<std::string> params;
    params.push_back(client->nickname);
    params.push_back(target_nickname);
    params.push_back(channel_name);
    std::string response = buildMessage("ircserv", "441", params, "They aren't on that channel");
    sendResponse(client_fd, response);
    return;
  }

	std::string kick_prefix = client->nickname + "!" + client->username + "@" + client->hostname;
	std::vector<std::string> kick_params;
	kick_params.push_back(channel_name);
	kick_params.push_back(target_nickname);
	std::string reason = command.trailing.empty() ? "Cause you are a larp dummy" : command.trailing;
	std::string kick_message = buildMessage(kick_prefix, "KICK", kick_params, reason);

	std::vector<int> channel_users = channel->getClientFds();
	broadcastResponse(kick_message, channel_users);
	channel->removeUser(target_fd);
}