/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 14:33:42 by nrobinso          #+#    #+#             */
/*   Updated: 2025/07/03 14:33:44 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::handleTopic(int client_fd, const Command &command) {
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

  if (command.trailing.empty()) 
    viewTopic(client_fd, channel_name);
  else 
    changeTopic(client_fd, channel_name, command.trailing);
  
}

void Server::viewTopic(int client_fd, const std::string &channel_name) {
  Client *client = clients[client_fd];
  Channel *channel = channels[channel_name];

  if (channel->getTopic().empty()) {
    std::vector<std::string> params;
    params.push_back(client->nickname);
    params.push_back(channel_name);
    std::string response = buildMessage("ircserv", "331", params, "No topic is set");
    sendResponse(client_fd, response);
  } else {
    std::vector<std::string> params;
    params.push_back(client->nickname);
    params.push_back(channel_name);
    std::string response = buildMessage("ircserv", "332", params, channel->getTopic());
    sendResponse(client_fd, response);

    if (!channel->topicSetBy.empty() && !channel->topicSetTime.empty()) {
      std::vector<std::string> whotime_params;
      whotime_params.push_back(client->nickname);
      whotime_params.push_back(channel_name);
      whotime_params.push_back(channel->topicSetBy);
      whotime_params.push_back(channel->topicSetTime);
      
      std::string whotime_response = buildMessage("ircserv", "333", whotime_params, "");
      sendResponse(client_fd, whotime_response);
    }
  }
}

void Server::changeTopic(int client_fd, const std::string &channel_name, const std::string &new_topic) {
  Client *client = clients[client_fd];
  Channel *channel = channels[channel_name];

  if (channel->topicRestricted && !channel->isOperator(client_fd)) {
    std::vector<std::string> params;
    params.push_back(client->nickname);
    params.push_back(channel_name);
    std::string response = buildMessage("ircserv", "482", params, "You're not channel operator");
    sendResponse(client_fd, response);
    return;
  }

  if (new_topic.empty() || new_topic == "\"\"") {
		std::cout << "new_topic is empty or \"\"" << std::endl;
    channel->setTopic("");
    channel->topicSetBy = "";
    channel->topicSetTime = "";
  } else {
    channel->setTopic(new_topic);
    channel->topicSetBy = client->nickname;
    channel->topicSetTime = getCurrentTime();
  }

  std::string topic_prefix = client->nickname + "!" + client->username + "@" + client->hostname;
  std::vector<std::string> topic_params;
  topic_params.push_back(channel_name);
  
  std::string topic_to_send = (new_topic.empty() || new_topic == "\"\"") ? "" : new_topic;
  std::string topic_message = buildMessage(topic_prefix, "TOPIC", topic_params, topic_to_send);

  std::vector<int> channel_users = channel->getClientFds();
  broadcastResponse(topic_message, channel_users);
}
