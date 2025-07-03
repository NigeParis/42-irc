/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 14:32:52 by nrobinso          #+#    #+#             */
/*   Updated: 2025/07/03 14:32:54 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::handlePing(int client_fd, const Command &command) {
  Client *client = clients[client_fd];
  std::string response = buildMessage("ircserv", "PONG", 
                                       std::vector<std::string>(1, client->nickname), 
                                       command.trailing);
  sendResponse(client_fd, response);
}