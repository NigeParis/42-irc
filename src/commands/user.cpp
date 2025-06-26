#include "../../include/Server.hpp"

void Server::handleUser(int client_fd, const Command &command) {
  if (clients.find(client_fd) == clients.end()) {
    return;
  }

  Client *client = clients[client_fd];
  Command cmd;
  cmd.prefix = "ircserv";

  if (client->username != "default" || client->hostname != "default") {
    cmd.name = "462";
    cmd.trailing = "You may not reregister";
    std::string response = buildMessage(cmd);
    sendResponse(client_fd, response);
    disconnectClient(client_fd);
    return;
  }

  if (command.params.size() < 3) {
    cmd.name = "461";
    cmd.trailing = "Not enough parameters";
    std::string response = buildMessage(cmd);
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

  Command cmd;
  cmd.prefix = "ircserv";
  cmd.params.push_back(nick);

  // 001 - Welcome + bannière
  cmd.name = "001";
  cmd.trailing = "Welcome to the IRC Network " + nick + "!" + client->username +
                 "@" + client->hostname + "\n" + putClientBanner();
  sendResponse(client_fd, buildMessage(cmd));

  // 002 - Host info
  cmd.name = "002";
  cmd.trailing = "Your host is ircserv, running version 1.0";
  sendResponse(client_fd, buildMessage(cmd));

  // 003 - Server creation time
  cmd.name = "003";
  cmd.trailing = "This server was created " + getCurrentTime();
  sendResponse(client_fd, buildMessage(cmd));

  // 004 - Server version & modes
  cmd.name = "004";
  cmd.params.push_back("ircserv");
  cmd.params.push_back("1.0");
  cmd.params.push_back("o");
  cmd.params.push_back("o");
  cmd.trailing.clear();
  sendResponse(client_fd, buildMessage(cmd));
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
