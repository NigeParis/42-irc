/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 14:33:51 by nrobinso          #+#    #+#             */
/*   Updated: 2025/07/03 14:33:53 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::handleUser(int client_fd, const Command &command) {
  if (clients.find(client_fd) == clients.end()) {
    return;
  }

  Client *client = clients[client_fd];

  if (client->username != "default" || client->hostname != "default") {
    std::string response = buildMessage("ircserv", "462",
                                       std::vector<std::string>(),
                                       "You may not reregister");
    sendResponse(client_fd, response);
    disconnectClient(client_fd);
    return;
  }

  if (command.params.size() < 3) {
    std::string response = buildMessage("ircserv", "461",
                                       std::vector<std::string>(),
                                       "Not enough parameters");
    sendResponse(client_fd, response);
    return;
  }

  client->username = command.params[0];
  client->hostname = command.params[1];
  sendWelcomeMessage(client_fd);
}

void Server::sendWelcomeMessage(int client_fd) {
  Client *client = clients[client_fd];
  std::string nick = client->nickname;

  // 001 - Welcome + bannière
  std::vector<std::string> params;
  params.push_back(nick);
  std::string welcome_msg = "Welcome to the IRC Network " + nick + "!" + client->username +
                           "@" + client->hostname + "\n" + putClientBanner();
  sendResponse(client_fd, buildMessage("ircserv", "001", params, welcome_msg));

  // 002 - Host info
  sendResponse(client_fd, buildMessage("ircserv", "002", params, "Your host is ircserv, running version 1.0"));

  // 003 - Server creation time
  sendResponse(client_fd, buildMessage("ircserv", "003", params, "This server was created " + getCurrentTime()));

  // 004 - Server version & modes
  std::vector<std::string> server_params;
  server_params.push_back(nick);
  server_params.push_back("ircserv");
  server_params.push_back("1.0");
  server_params.push_back("o");
  server_params.push_back("o");
  sendResponse(client_fd, buildMessage("ircserv", "004", server_params, ""));
}

std::string Server::putClientBanner(void) {

  std::stringstream ss;

  ss << "██╗██████╗  ██████╗     ██████╗██╗     ██╗███████╗███╗   ██╗████████╗ "
        "\n";
  ss << "██║██╔══██╗██╔════╝    ██╔════╝██║     ██║██╔════╝████╗  ██║╚══██╔══╝ "
        "\n";
  ss << "██║██████╔╝██║         ██║     ██║     ██║█████╗  ██╔██╗ ██║   ██║ \n";
  ss << "██║██╔══██╗██║         ██║     ██║     ██║██╔══╝  ██║╚██╗██║   ██║ \n";
  ss << "██║██║  ██║╚██████╗    ╚██████╗███████╗██║███████╗██║ ╚████║   ██║ \n";
  ss << "══════════════════════════════════════════════════════════════════ \n";

  return (ss.str());
};
