#include "../../include/Server.hpp"

void Server::handleQuit(int client_fd, const Command &command) {
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
  
  std::string quit_reason = "Quit: ";
  if (!command.trailing.empty()) {
    quit_reason += command.trailing;
  } else if (!command.params.empty()) {
    quit_reason += command.params[0];
  }

  std::string quit_prefix = client->nickname + "!" + client->username + "@" + client->hostname;
  std::string quit_message = buildMessage(quit_prefix, "QUIT", std::vector<std::string>(), quit_reason);

  std::set<int> clients_to_notify = getChannelsClientList(client_fd);
  
  clients_to_notify.erase(client_fd);

  broadcastResponse(quit_message, std::vector<int>(clients_to_notify.begin(), clients_to_notify.end()));

  for (std::map<std::string, Channel *>::iterator it = channels.begin();
       it != channels.end(); ++it) {
    Channel *channel = it->second;
    channel->removeUserFromOperators(client_fd);
    channel->removeUserFromInvitedUsers(client_fd);
  }

  disconnectClient(client_fd);
}