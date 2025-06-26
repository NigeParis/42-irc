/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:54:04 by nige42            #+#    #+#             */
/*   Updated: 2025/06/21 11:17:33 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <cstddef>
#include <sstream>
#include <vector>

Server::Server(int port, std::string password)
    : port_(port), password_(password), lastClientToWrite_(0) {};

Server::~Server() {
  std::cout << BLUE << "[DEBUG] - destructor Server" << RESET << std::endl;

  close(this->socket_);
  // TODO: correctly cleanupUsers();
  for (std::map<int, Client *>::iterator it = clients.begin();
       it != clients.end(); ++it) {
    close(it->first);
    delete it->second;
  }
  clients.clear();

  for (std::map<std::string, Channel *>::iterator it = channels.begin();
       it != channels.end(); ++it) {
    delete it->second;
  }
  channels.clear();

  // TODO: cleanupChannels();
};

void Server::createServer(void) {

  struct sockaddr_in addressServer;
  int opt = 1;

  this->socket_ = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(this->socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  memset(&addressServer, 0, sizeof(addressServer));
  addressServer.sin_family = AF_INET;
  addressServer.sin_addr.s_addr = INADDR_ANY;
  addressServer.sin_port = htons(this->port_);
  if (bind(this->socket_, (struct sockaddr *)&addressServer,
           sizeof(addressServer)) < 0) {
    std::cout << "Error: binding socket" << std::endl;
    close(this->socket_);
    exit(EXIT_FAILURE);
  }
  listen(this->socket_, 10);
  putServerBanner();
};

void Server::userLoopCheck() {

  epoll_event events[BUFFER];

  int epfd = epoll_create1(0);
  if (epfd == -1) {
    perror("epoll_create failed");
    return;
  }

  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = this->socket_;
  epoll_ctl(epfd, EPOLL_CTL_ADD, this->socket_, &ev);
  while (SigHandler::sigloop) {
    int num_events = epoll_wait(epfd, events, 10, 1000);
    if (num_events < 0) {
      if (!SigHandler::sigloop)
        break;
      perror("epoll_wait failed");
    }
    for (int i = 0; i < num_events; ++i) {
      int fd = events[i].data.fd;
      if (fd == this->socket_ && (events[i].events & EPOLLIN)) {
        acceptNewClient(epfd);
      } else if (events[i].events & EPOLLIN) {
        if ((events[i].events & (EPOLLERR | EPOLLHUP)) == 0) {
          handleClientMessage(fd);
        }
      }
    }
  }

  close(epfd);
}

void Server::acceptNewClient(int epfd) {
  struct sockaddr_in clientAddress;
  socklen_t clientAddressLength = sizeof(clientAddress);
  int client_fd = accept(this->socket_, (struct sockaddr *)&clientAddress,
                         &clientAddressLength);
  if (client_fd < 0) {
    perror("accept failed");
    return;
  }

  if (!setNonBlocking(client_fd)) {
    close(client_fd);
    return;
  }

  struct epoll_event user_ev;
  user_ev.events = EPOLLIN;
  user_ev.data.fd = client_fd;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &user_ev) < 0) {
    perror("epoll_ctl failed");
    close(client_fd);
    return;
  }

  lastClientToWrite_++;
  clients[client_fd] = new Client(client_fd, lastClientToWrite_);

  std::cout << GREEN << "[DEBUG] - new client connected: " << client_fd << RESET
            << std::endl;
}

void Server::handleClientMessage(int client_fd) {
  if (clients.find(client_fd) == clients.end()) {
    std::cout << RED << "[ERROR] - client not found: " << client_fd << RESET
              << std::endl;
    return;
  }

  char buffer[BUFFER];
  memset(buffer, 0, BUFFER);
  ssize_t bytes_read = recv(client_fd, buffer, BUFFER - 1, 0);
  if (bytes_read < 0) {
    perror("recv failed");
    return;
  }
  if (bytes_read == 0) {
    std::cout << RED << "[DEBUG] - client disconnected: " << client_fd << RESET
              << std::endl;
    disconnectClient(client_fd);
    return;
  }
  buffer[bytes_read] = '\0';
  Client *client = clients[client_fd];
  client->buffer += buffer;

  if (client->buffer.substr(client->buffer.size() - 2) != "\r\n") {
    return;
  }

  std::cout << BLUE
            << "[DEBUG] - received message from client: " << client->buffer
            << RESET << std::endl;
  std::cout << "==================================" << std::endl;

  std::vector<std::string> commands = split(client->buffer, "\r\n");
  client->buffer.clear();

  for (size_t i = 0; i < commands.size() - 1; ++i) {

    Command command = parseCommand(commands[i]);
    std::cout << GREEN << "[DEBUG] - command received: " << command.name
              << RESET << std::endl;

    if (command.name == "PING")
      handlePing(client_fd, command);
    else if (command.name == "CAP")
      handleCap(client_fd, command);
    else if (command.name == "PONG")
      handlePong(client_fd, command);
    else if (command.name == "NICK")
      handleNick(client_fd, command);
    else if (command.name == "USER")
      handleUser(client_fd, command);
    else if (command.name == "PASS")
      handlePass(client_fd, command);
    else if (command.name == "JOIN")
      handleJoin(client_fd, command);
    else if (command.name == "PART")
      handlePart(client_fd, command);
    else if (command.name == "PRIVMSG")
      handlePrivmsg(client_fd, command);
    else if (command.name == "MODE")
      handleMode(client_fd, command);
    else if (command.name == "TOPIC")
      handleTopic(client_fd, command);
    else if (command.name == "KICK")
      handleKick(client_fd, command);
    else if (command.name == "INVITE")
      handleInvite(client_fd, command);
    else if (command.name == "QUIT")
      handleQuit(client_fd, command);
    else
      handleUnknownCommand(client_fd, command);
  }
}

void Server::disconnectClient(int client_fd) {
  if (clients.find(client_fd) == clients.end()) {
    std::cout << RED << "[ERROR] - client not found: " << client_fd << RESET
              << std::endl;
    return;
  }
  close(client_fd);
  delete clients[client_fd];
  clients.erase(client_fd);
  std::cout << RED << "[DEBUG] - client disconnected: " << client_fd << RESET
            << std::endl;
}

Command Server::parseCommand(const std::string &input) {
  Command command;
  std::istringstream iss(input);
  std::string token;

  if (input[0] == ':') {
    iss >> token;
    command.prefix = token.substr(1);
  }

  iss >> command.name;

  while (iss >> token) {
    if (token[0] == ':') {
      command.trailing = token.substr(1);
      std::string word;
      while (iss >> word) {
        command.trailing += " " + word;
      }
      break;
    }
    command.params.push_back(token);
  }
  return command;
}

void Server::handlePing(int client_fd, const Command &command) {
  (void)command;   // Unused parameter
  (void)client_fd; // Unused parameter
}

void Server::handleCap(int client_fd, const Command &command) {
  std::cout << BLUE << "[DEBUG] - case: CAP " << command.params[0] << RESET
            << std::endl;
  if (command.params[0] != "LS")
    return;
  std::cout << "==================================" << std::endl;

  sendResponse(client_fd, "CAP * LS \r\n");
}

void Server::handlePong(int client_fd, const Command &command) {
  (void)command;   // Unused parameter
  (void)client_fd; // Unused parameter
  std::cout << BLUE << "[DEBUG] - case: PONG" << RESET << std::endl;
  std::cout << "params: ";
  for (size_t i = 0; i < command.params.size(); ++i) {
    std::cout << command.params[i] << " ";
  };
  std::cout << std::endl;
  std::cout << "==================================" << std::endl;
}

void Server::handleNick(int client_fd, const Command &command) {
  std::cout << BLUE << "[DEBUG] - case: NICK" << RESET << std::endl;
  std::cout << "params: ";
  for (size_t i = 0; i < command.params.size(); ++i) {
    std::cout << command.params[i] << " ";
  };
  std::cout << std::endl;
  std::cout << "==================================" << std::endl;
}

void Server::handleJoin(int client_fd, const Command &command) {
  (void)command;   // Unused parameter
  (void)client_fd; // Unused parameter
}

void Server::handlePart(int client_fd, const Command &command) {
  (void)command;   // Unused parameter
  (void)client_fd; // Unused parameter
}

void Server::handlePrivmsg(int client_fd, const Command &command) {
  (void)command;   // Unused parameter
  (void)client_fd; // Unused parameter
}

void Server::handleQuit(int client_fd, const Command &command) {
  (void)command; // Unused parameter
  std::cout << BLUE << "[DEBUG] - case: QUIT" << RESET << std::endl;
  disconnectClient(client_fd);
}

void Server::handlePass(int client_fd, const Command &command) {

  Client *client = clients[client_fd];
  Command cmd;
  cmd.prefix = "ircserv";
  cmd.params.push_back(client->nickname);

  if (client->is_authenticated) {

    cmd.name = "462";
    cmd.trailing = "You may not reregister";
    std::string response = buildMessage(cmd);
    sendResponse(client_fd, response);
    disconnectClient(client_fd);
    return;
  }
  if (command.params.size() < 1) {
    cmd.name = "461";
    cmd.trailing = "Not enough parameters";
    std::string response = buildMessage(cmd);
    sendResponse(client_fd, response);
    return;
  }
  if (command.params[0] != password_) {
    cmd.name = "464";
    cmd.trailing = "Password incorrect";
    std::string response = buildMessage(cmd);
    sendResponse(client_fd, response);
    disconnectClient(client_fd);
    return;
  }

  std::cout << BLUE << "[DEBUG] client reregistered succesfully" << RESET
            << std::endl;
  client->is_authenticated = true;
  return;
}

void Server::handleUser(int client_fd, const Command &command) {
  (void)command;   // Unused parameter
  (void)client_fd; // Unused parameter
}

void Server::handleMode(int client_fd, const Command &command) {
  (void)command;   // Unused parameter
  (void)client_fd; // Unused parameter
}

void Server::handleTopic(int client_fd, const Command &command) {
  (void)command;   // Unused parameter
  (void)client_fd; // Unused parameter
}

void Server::handleKick(int client_fd, const Command &command) {
  (void)command;   // Unused parameter
  (void)client_fd; // Unused parameter
}

void Server::handleInvite(int client_fd, const Command &command) {
  (void)command;   // Unused parameter
  (void)client_fd; // Unused parameter
}

void Server::handleUnknownCommand(int client_fd, const Command &command) {
  std::string response = "Unknown command: " + command.name + "\r\n";
  sendResponse(client_fd, response);
}

std::string Server::buildMessage(const Command &cmd) {
  std::ostringstream oss;

  if (!cmd.prefix.empty()) {
    oss << ":" << cmd.prefix << " ";
  }

  oss << cmd.name;

  for (size_t i = 0; i < cmd.params.size(); ++i) {
    oss << " " << cmd.params[i];
  }

  if (!cmd.trailing.empty()) {
    oss << " :" << cmd.trailing;
  }

  oss << "\r\n";

  return oss.str();
}

void Server::sendResponse(int client_fd, const std::string &response) {
  if (clients.find(client_fd) == clients.end()) {
    std::cout << RED << "[ERROR] - client not found: " << client_fd << RESET
              << std::endl;
    return;
  }
  Client *client = clients[client_fd];
  ssize_t bytes_sent = send(client_fd, response.c_str(), response.size(), 0);
  if (bytes_sent <= 0) {
    perror("send failed");
    return;
  }
  client->buffer.clear();
}

void Server::broadcastResponse(const std::string &message,
                               const std::vector<int> &client_fds) {
  for (size_t i = 0; i < client_fds.size(); ++i) {
    sendResponse(client_fds[i], message);
  }
}

bool Server::setNonBlocking(int fd) {
  if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
    perror("fcntl O_NONBLOCK failed");
    return false;
  }
  return true;
}

std::string Server::getCurrentTime() {
  std::time_t now = std::time(NULL);
  std::tm *localTime = std::localtime(&now);

  char buffer[80];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);

  return std::string(buffer);
}

void Server::putServerBanner(void) {

  std::cout << "██████╗███████╗██████╗ ██╗   ██╗███████╗██████╗     ██╗██████╗ "
               " ██████╗ "
            << std::endl;
  std::cout << "█╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗    "
               "██║██╔══██╗██╔════╝ "
            << std::endl;
  std::cout
      << "██████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝    ██║██████╔╝██║"
      << std::endl;
  std::cout
      << "════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗    ██║██╔══██╗██║"
      << std::endl;
  std::cout << "██████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║    ██║██║  "
               "██║╚██████╗ "
            << std::endl;
  std::cout << "══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝    ╚═╝╚═╝  "
               "╚═╝ ╚═════╝ "
            << std::endl;
  std::cout << "═══════════════════════════════════════════════════════════════"
               "════════ "
            << std::endl;
  std::cout << "server listening on port: " << this->port_ << std::endl;
  std::cout << "server password: " << this->password_ << std::endl;
  std::cout << std::endl;
  std::cout << YELLOW << "Users connected: " << clients.size() << RESET
            << std::endl;
  std::cout << "═══════════════════════════════════════════════════════════════"
               "════════ "
            << std::endl;
};

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
