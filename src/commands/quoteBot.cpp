#include "../../include/Server.hpp"

void Server::handleQuote(int client_fd, const Command &command) {
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

  if (command.params.size() == 0) {
    getQuote(client_fd);
  } else if (command.params.size() == 1 && command.params[0] == "set") {
    setQuote(client_fd, command);
  } else if (command.params.size() == 1 && command.params[0] == "remove") {
    removeQuote(client_fd, command);
  } else if (command.params.size() == 1 && command.params[0] == "list") {
    listQuotes(client_fd);
  }
}

void Server::getQuote(int client_fd) {
  Client *client = clients[client_fd];
  std::string response = buildMessage("ircserv", "QUOTE", 
                                       std::vector<std::string>(1, client->nickname), 
                                       client->quote.getRandomQuote());
  sendResponse(client_fd, response);
}

void Server::setQuote(int client_fd, const Command &command) { 
	Client *client = clients[client_fd];
	if (command.params.size() < 1) {
    std::string response = buildMessage("ircserv", "461", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "Not enough parameters");
    sendResponse(client_fd, response);
    return;
  }
  client->quote.setQuote(command.trailing);
  std::string response = buildMessage("ircserv", "QUOTE", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "Quote set");
  sendResponse(client_fd, response);
}

void Server::removeQuote(int client_fd, const Command &command) {
	Client *client = clients[client_fd];
	if (command.params.size() < 1) {
    std::string response = buildMessage("ircserv", "461", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "Not enough parameters");
    sendResponse(client_fd, response);
    return;
  }
  client->quote.removeQuote(command.trailing);
  std::string response = buildMessage("ircserv", "QUOTE", 
                                       std::vector<std::string>(1, client->nickname), 
                                       "Quote removed");
  sendResponse(client_fd, response);
}

void Server::listQuotes(int client_fd) {
	Client *client = clients[client_fd];
	std::vector<std::string> quotes = client->quote.getQuotes();
	for (size_t i = 0; i < quotes.size(); i++) {
		std::string response = buildMessage("ircserv", "QUOTE", 
											std::vector<std::string>(), 
											quotes[i]);
		sendResponse(client_fd, response);
	}
}