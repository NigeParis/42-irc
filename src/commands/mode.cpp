/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 14:32:09 by nrobinso          #+#    #+#             */
/*   Updated: 2025/07/03 14:32:11 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::handleMode(int client_fd, const Command &command) {
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
    std::string response = buildMessage("ircserv", "461", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "Not enough parameters");
    sendResponse(client_fd, response);
    return;
  }

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

  if (command.params.size() == 1) {
    sendCurrentChannelModes(client_fd, channel_name);
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

  std::string modestring = command.params[1];
  std::vector<std::string> mode_params;
  
  for (size_t i = 2; i < command.params.size(); i++) {
    mode_params.push_back(command.params[i]);
  }

  applyModeChanges(client_fd, channel_name, modestring, mode_params);
}

void Server::sendCurrentChannelModes(int client_fd, const std::string &channel_name) {
  Client *client = clients[client_fd];
  Channel *channel = channels[channel_name];
  
  std::string modes = "+";
  std::vector<std::string> mode_params;
  
  if (channel->isInviteOnly) modes += "i";
  if (channel->topicRestricted) modes += "t";
  if (!channel->password.empty()) modes += "k"; 
  if (channel->userLimit > 0) {
    modes += "l";
    mode_params.push_back(intToString(channel->userLimit));
  }
  
  if (modes == "+") modes = "";
  
  std::vector<std::string> params;
  params.push_back(client->nickname);
  params.push_back(channel_name);
  params.push_back(modes);
  
  for (size_t i = 0; i < mode_params.size(); i++) {
    params.push_back(mode_params[i]);
  }
  
  std::string response = buildMessage("ircserv", "324", params, "");
  sendResponse(client_fd, response);
  
  std::vector<std::string> time_params;
  time_params.push_back(client->nickname);
  time_params.push_back(channel_name);
  time_params.push_back(channel->creationTime);
  std::string time_response = buildMessage("ircserv", "329", time_params, "");
  sendResponse(client_fd, time_response);
}

void Server::applyModeChanges(int client_fd, const std::string &channel_name, 
                             const std::string &modestring, 
                             const std::vector<std::string> &mode_params) {
  
  Channel *channel = channels[channel_name];
  Client *client = clients[client_fd];
  
  bool adding = true;
  size_t param_index = 0;
  std::string applied_modes = "";
  std::vector<std::string> applied_params;
  std::string current_prefix = "";
  
  for (size_t i = 0; i < modestring.length(); i++) {
    char mode_char = modestring[i];
    
    if (mode_char == '+') {
      adding = true;
      if (current_prefix != "+") {
        applied_modes += "+";
        current_prefix = "+";
      }
      continue;
    }
    
    if (mode_char == '-') {
      adding = false;
      if (current_prefix != "-") {
        applied_modes += "-";
        current_prefix = "-";
      }
      continue;
    }
    
    switch (mode_char) {
      case 'i':
        if (adding != channel->isInviteOnly) {
          channel->isInviteOnly = adding;
          applied_modes += mode_char;
        }
        break;
        
      case 't':
        if (adding != channel->topicRestricted) {
          channel->topicRestricted = adding;
          applied_modes += mode_char;
        }
        break;
        
      case 'k':
        if (adding) {
          if (param_index < mode_params.size()) {
            channel->password = mode_params[param_index];
            channel->isPrivate = true;
            applied_modes += mode_char;
            applied_params.push_back(mode_params[param_index]);
            param_index++;
          }
        } else {
          if (!channel->password.empty()) {
            channel->password = "";
            channel->isPrivate = false;
            applied_modes += mode_char;
          }
        }
        break;
        
      case 'o':
        if (param_index < mode_params.size()) {
          int target_fd = findClientByNickname(mode_params[param_index]);
          if (target_fd != -1 && channel->hasUser(target_fd)) {
            if (adding) {
              if (!channel->isOperator(target_fd)) {
                channel->addUserToOperators(target_fd);
                applied_modes += mode_char;
                applied_params.push_back(mode_params[param_index]);
              }
            } else {
              if (channel->isOperator(target_fd)) {
                channel->removeUserFromOperators(target_fd);
                applied_modes += mode_char;
                applied_params.push_back(mode_params[param_index]);
              }
            }
          }
          param_index++;
        }
        break;
        
      case 'l':
        if (adding) {
          if (param_index < mode_params.size()) {
            int limit = atoi(mode_params[param_index].c_str());
            if (limit > 0) {
              channel->userLimit = limit;
              applied_modes += mode_char;
              applied_params.push_back(mode_params[param_index]);
            }
            param_index++;
          }
        } else {
          if (channel->userLimit > 0) {
            channel->userLimit = -1;
            applied_modes += mode_char;
          }
        }
        break;
        
      default:
        std::vector<std::string> params;
        params.push_back(client->nickname);
        std::string response = buildMessage("ircserv", "501", params, "Unknown MODE flag");
        sendResponse(client_fd, response);
        break;
    }
  }
  
  if (!applied_modes.empty() && applied_modes != "+" && applied_modes != "-") {
    std::string mode_prefix = client->nickname + "!" + client->username + "@" + client->hostname;
    std::vector<std::string> broadcast_params;
    broadcast_params.push_back(channel_name);
    broadcast_params.push_back(applied_modes);
    
    for (size_t i = 0; i < applied_params.size(); i++) {
      broadcast_params.push_back(applied_params[i]);
    }
    
    std::string mode_message = buildMessage(mode_prefix, "MODE", broadcast_params, "");
    std::vector<int> channel_users = channel->getClientFds();
    broadcastResponse(mode_message, channel_users);
  }
}