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

#include "../include/server.hpp"

Server::Server(int port, std::string password)
    : port_(port), password_(password), lastClientToWrite_(0) {};

Server::~Server() {
  std::cout << BLUE << "[DEBUG] - destructor Server" << RESET << std::endl;

  close(this->socket_);
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

  // cleanupChannels();
  this->channels_.clear();
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

  clients[client_fd] = new Client(client_fd);

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
  if (bytes_read <= 0) {
    perror("recv failed");
    return;
  }
  buffer[bytes_read] = '\0';
  Client *client = clients[client_fd];
  client->buffer += buffer;
  if (client->buffer.substr(client->buffer.size() - 2) != "\r\n") {
    return;
  }

  Command command = parseCommand(client->buffer);

  if (command.name == "PING")
    handlePing(client_fd, command);
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
    putErrorMessage(client_fd, command.name, "Unknown command", 421);
}

std::string Server::buildResponse(const Command &command, User *user) {
  return "hello world";
};

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
  for (int i = 0; i < client_fds.size(); ++i) {
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
  std::cout << YELLOW << "Users connected: " << users_.size() << RESET
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

void Server::timeStamp(void) {

  std::time_t now = std::time(NULL);
  std::tm *localTime = std::localtime(&now);

  std::cout << std::setfill('0') << std::setw(2) << localTime->tm_hour << ":"
            << std::setw(2) << localTime->tm_min << ":" << std::setw(2)
            << localTime->tm_sec << " ";
};
