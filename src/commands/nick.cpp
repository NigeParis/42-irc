#include "../../include/Server.hpp"

void Server::handleNick(int client_fd, const Command &command) {
  if (clients.find(client_fd) == clients.end()) {
    return;
  }

  Client *client = clients[client_fd];
  Command cmd;
  cmd.prefix = "ircserv";

  if (!client->is_authenticated) {
    cmd.name = "451";
    cmd.params.push_back(client->nickname);
    cmd.trailing = "You have not registered";
    std::string response = buildMessage(cmd.prefix, cmd.name, cmd.params, cmd.trailing);
    sendResponse(client_fd, response);
    return;
  }

  if (command.params.size() < 1) {
    cmd.name = "431";
    cmd.params.push_back(client->nickname);
    cmd.trailing = "No nickname given";
    std::string response = buildMessage(cmd.prefix, cmd.name, cmd.params, cmd.trailing);
    sendResponse(client_fd, response);
    return;
  }

  if (!isValidNickname(command.params[0])) {
    cmd.name = "432";
    cmd.params.push_back(client->nickname);
    cmd.trailing = "Erroneous nickname";
    std::string response = buildMessage(cmd.prefix, cmd.name, cmd.params, cmd.trailing);
    sendResponse(client_fd, response);
    return;
  }

  if (!isAvailableNickname(command.params[0])) {
    cmd.name = "433";
    cmd.params.push_back(client->nickname);
    cmd.trailing = "Nickname is already in use";
    std::string response = buildMessage(cmd.prefix, cmd.name, cmd.params, cmd.trailing);
    sendResponse(client_fd, response);
    return;
  }
  cmd.prefix =
      client->nickname + "!" + client->username + "@" + client->hostname;
  cmd.name = "NICK";
  cmd.trailing = command.params[0];
  std::string response = buildMessage(cmd.prefix, cmd.name, cmd.params, cmd.trailing);
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
