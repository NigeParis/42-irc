/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 14:32:18 by nrobinso          #+#    #+#             */
/*   Updated: 2025/07/03 14:32:20 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::handleNick(int client_fd, const Command &command) {
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

  if (command.params.size() < 1) {
    std::string response = buildMessage("ircserv", "431", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "No nickname given");
    sendResponse(client_fd, response);
    return;
  }

  if (!isValidNickname(command.params[0])) {
    std::string response = buildMessage("ircserv", "432", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "Erroneous nickname");
    sendResponse(client_fd, response);
    return;
  }

  if (!isAvailableNickname(command.params[0])) {
    std::string response = buildMessage("ircserv", "433", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "Nickname is already in use");
    sendResponse(client_fd, response);
    return;
  }

  std::string prefix = client->nickname + "!" + client->username + "@" + client->hostname;
  std::string response = buildMessage(prefix, "NICK", 
                                     std::vector<std::string>(), 
                                     command.params[0]);
  client->nickname = command.params[0];

  std::cout << "Client " << client_fd << " changed nickname to "
            << client->nickname << std::endl;

  std::set<int> client_fds = getChannelsClientList(client_fd);

  broadcastResponse(response,
                    std::vector<int>(client_fds.begin(), client_fds.end()));
}

bool Server::isAvailableNickname(const std::string &nickname) {
  for (std::map<int, Client *>::iterator it = clients.begin();
       it != clients.end(); ++it) {
    if (it->second->nickname == nickname) {
      return false;
    }
  }
  return true;
}

bool Server::isValidNickname(const std::string &nickname) {
  if (nickname.empty())
    return false;

  if (std::isdigit(nickname[0]))
    return false;

  if (nickname[0] == '#' || nickname[0] == ':' ||
      nickname.find(' ') != std::string::npos)
    return false;

  for (size_t i = 0; i < nickname.length(); ++i) {
    char c = nickname[i];
    if (!std::isalnum(c) && c != '[' && c != ']' && c != '{' && c != '}' &&
        c != '\\' && c != '|' && c != '-' && c != '_') {
      return false;
    }
  }

  return true;
}
