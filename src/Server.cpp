/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:54:04 by nige42            #+#    #+#             */
/*   Updated: 2025/07/03 16:08:46 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <cstddef>
#include <sstream>
#include <vector>

Server::Server(int port, std::string password)
    : port_(port), password_(password), lastClientToWrite_(0) {};

Server::~Server() {

  close(this->socket_);
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
};

void Server::createServer(void) {

  struct sockaddr_in addressServer;                           // Declare a structure to hold IPv4 socket address information
  int opt = 1;

  this->socket_ = socket(AF_INET, SOCK_STREAM, 0);            // Create a new socket using IPv4 and TCP
                                                              // AF_INET     → Address family for IPv4
                                                              // SOCK_STREAM → Type for TCP (stream-based, reliable connection)
                                                              // 0           → Default protocol (TCP for SOCK_STREAM)
  setsockopt(this->socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));   // Set socket option to allow address reuse
                                                                            // SOL_SOCKET    → Option level for socket layer
                                                                            // SO_REUSEADDR  → Allows reuse of local address (prevents "Address already in use" error)
                                                                            // &opt          → Pointer to an int (usually set to 1)
                                                                            // sizeof(opt)   → Size of the option value
  memset(&addressServer, 0, sizeof(addressServer));           // Clear the addressServer structure to avoid garbage values
                                                              // Ensures all fields are zeroed before assigning specific values
  addressServer.sin_family = AF_INET;                         // Set the address family to IPv4
  addressServer.sin_addr.s_addr = INADDR_ANY;                 // Accept connections on available network interfaces e.g., localhost, Wi-Fi, Ethernet, etc.)
                                                              // addressServer.sin_addr.s_addr = inet_addr("127.0.0.1");
  addressServer.sin_port = htons(this->port_);                // Convert the port number from host byte order to network byte order
  if (bind(this->socket_, (struct sockaddr *)&addressServer,  // Attempt to bind the socket to the specified IP address and port
           sizeof(addressServer)) < 0) {
    std::cout << "Error: binding socket" << std::endl;
    close(this->socket_);
    exit(EXIT_FAILURE);
  }
  listen(this->socket_, 10);                                  // Mark the socket as passive, ready to accept incoming connections
                                                              // The second argument (10) is the backlog:
                                                              // → It defines the maximum number of pending connections that can be queued
                                                              // → If more than 10 clients try to connect before accept() is called, new connections may be refused

  putServerBanner();
};

void Server::userLoopCheck() {

  epoll_event events[BUFFER];
                                // Create a new epoll instance and return its file descriptor
                                // epoll_create1(0) is equivalent to epoll_create(), but without the obsolete 'size' argument
                                // The argument '0' means no special flags are set (e.g., no EPOLL_CLOEXEC)
  int epfd = epoll_create1(0);
  if (epfd == -1) {
    perror("epoll_create failed");
    return;
  }

  struct epoll_event ev;  // Declare an epoll_event structure to describe the event we're interested in
  ev.events = EPOLLIN;    // Specify the type of event to monitor:
                          // EPOLLIN → Trigger when the file descriptor is ready for reading
  ev.data.fd = this->socket_;   // Associate the event with a specific file descriptor (e.g., the listening socket)
  epoll_ctl(epfd, EPOLL_CTL_ADD, this->socket_, &ev); // Register the file descriptor with the epoll instance
                                                      // epfd            → epoll instance file descriptor (created with epoll_create1())
                                                      // EPOLL_CTL_ADD   → Operation to add a new file descriptor to the interest list
                                                      // this->socket_   → The file descriptor to monitor (e.g., server socket)
                                                      // &ev             → Pointer to the epoll_event structure describing what to watch
  while (SigHandler::sigloop) {
    int num_events = epoll_wait(epfd, events, 10, 1000);  // Wait for I/O events on the epoll instance
                                                          // epfd       → File descriptor returned by epoll_create1()
                                                          // events     → Array of epoll_event structs to store triggered events
                                                          // 10         → Maximum number of events to return (array size)
                                                          // 1000       → Timeout in milliseconds (1000 ms = 1 second)
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
                         &clientAddressLength);                             // Accept an incoming connection on the listening socket
                                                                            // this->socket_         → The listening socket file descriptor
                                                                            // (struct sockaddr *)&clientAddress → Pointer to a sockaddr_in structure that will be filled with the client's address info
                                                                            // &clientAddressLength  → Pointer to a variable holding the size of clientAddress; updated with actual size on return
                                                                            // - accept() extracts the first pending connection from the queue of this->socket_
                                                                            // - It creates a new socket (client_fd) specifically for communicating with that client
                                                                            // - The original socket remains open and continues listening for new connections
                                                                            // - If no connections are pending and the socket is blocking, accept() will block until one arrives
                                                                            // - If the socket is non-blocking and no connections are pending, accept() returns -1 and sets errno to EAGAIN or EWOULDBLOCK
                                                                            // - The returned client_fd should be used for all communication with the connected client
  if (client_fd < 0) {
    perror("accept failed");
    return;
  }

  if (!setNonBlocking(client_fd)) {
    close(client_fd);
    return;
  }

  struct epoll_event user_ev;   // Create an epoll_event structure to describe what we want to monitor
  user_ev.events = EPOLLIN;     // Set the event type to EPOLLIN
                                // → This means we want to be notified when client_fd is ready for reading
  user_ev.data.fd = client_fd;  // Associate the event with the client's file descriptor
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &user_ev) < 0) {  // Register the client_fd with the epoll instance
                                                                  // epfd           → epoll instance file descriptor
                                                                  // EPOLL_CTL_ADD  → Operation to add a new file descriptor to the interest list
                                                                  // client_fd      → The file descriptor to monitor (e.g., a connected client socket)
                                                                  // &user_ev       → Pointer to the event configuration
    perror("epoll_ctl failed");
    close(client_fd);
    return;
  }

  lastClientToWrite_++;
  clients[client_fd] = new Client(client_fd, lastClientToWrite_);
}

void Server::handleClientMessage(int client_fd) {
  if (clients.find(client_fd) == clients.end()) {
    return;
  }

  char buffer[BUFFER];
  memset(buffer, 0, BUFFER);
  ssize_t bytes_read = recv(client_fd, buffer, BUFFER - 1, 0);  // Receive data from the connected socket
                                                                // client_fd   : file descriptor of the client socket
                                                                // buffer      : char array to store the received data
                                                                // BUFFER - 1  : max number of bytes to read (leaves space for '\0')
                                                                // 0           : flags (0 = default behavior)

                                                                // Return:
                                                                // > 0  -> number of bytes successfully received
                                                                // = 0  -> connection gracefully closed by peer
                                                                // < 0  -> error occurred (check errno for details)
  if (bytes_read < 0) {
    perror("recv failed");
    return;
  }
  if (bytes_read == 0) {
    disconnectClient(client_fd);
    return;
  }
  buffer[bytes_read] = '\0';                                    // Optional: null-terminate buffer if you're treating it as a string
  Client *client = clients[client_fd];
  client->buffer += buffer;

  if (client->buffer.substr(client->buffer.size() - 2) != "\r\n") {
    return;
  }

  std::vector<std::string> commands = split(client->buffer, "\r\n");
  client->buffer.clear();

  for (size_t i = 0; i < commands.size(); ++i) {
    Command command = parseCommand(commands[i]);
    std::cout << "Received command: " << commands[i]
              << " from client fd: " << client_fd << std::endl;

    if (command.name == "CAP")
      handleCap(client_fd, command);
    else if (command.name == "NICK")
      handleNick(client_fd, command);
    else if (command.name == "USER")
      handleUser(client_fd, command);
    else if (command.name == "PASS")
      handlePass(client_fd, command);
    else if (command.name == "JOIN")
      handleJoin(client_fd, command);
    else if (command.name == "QUIT")
      handleQuit(client_fd, command);
    else if (command.name == "PART")
      handlePart(client_fd, command);
    else if (command.name == "PRIVMSG")
      handlePrivmsg(client_fd, command);
    else if (command.name == "NOTICE")
      handleNotice(client_fd, command);
    else if (command.name == "MODE")
      handleMode(client_fd, command);
    else if (command.name == "TOPIC")
      handleTopic(client_fd, command);
    else if (command.name == "KICK")
      handleKick(client_fd, command);
    else if (command.name == "INVITE")
      handleInvite(client_fd, command);
    else if (command.name == "PING")
      handlePing(client_fd, command);
    else if (command.name == "bot")
      handleQuote(client_fd, command);
    else
      handleUnknownCommand(client_fd, command);
  }
}

void Server::disconnectClient(int client_fd) {
  if (clients.find(client_fd) == clients.end()) {
    return;
  }
  close(client_fd);
  delete clients[client_fd];
  clients.erase(client_fd);
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

void Server::handleCap(int client_fd, const Command &command) {
  if (command.params[0] != "LS")
    return;

  sendResponse(client_fd, "CAP * LS \r\n");
}

void Server::handleUnknownCommand(int client_fd, const Command &command) {
  std::string response = "Unknown command: " + command.name + "\r\n";
  sendResponse(client_fd, response);
}

std::set<int> Server::getChannelsClientList(int client_fd) {
  std::set<int> client_fds;
  client_fds.insert(client_fd);

  for (std::map<std::string, Channel *>::iterator it = channels.begin();
       it != channels.end(); ++it) {
    Channel *channel = it->second;
    if (channel->hasUser(client_fd)) {
      std::vector<int> channel_fds = channel->getClientFds();
      client_fds.insert(channel_fds.begin(), channel_fds.end());
    }
  }
  return client_fds;
}

bool Server::isChannelExists(const std::string &channel_name) {
  return channels.find(channel_name) != channels.end();
}

std::string Server::buildMessage(const std::string &prefix,
                                 const std::string &name,
                                 const std::vector<std::string> &params,
                                 const std::string &trailing = "") {
  std::ostringstream oss;

  if (!prefix.empty()) {
    oss << ":" << prefix << " ";
  }

  oss << name;

  for (size_t i = 0; i < params.size(); ++i) {
    oss << " " << params[i];
  }

  if (!trailing.empty()) {
    oss << " :" << trailing;
  }

  oss << "\r\n";

  return oss.str();
}

void Server::sendResponse(int client_fd, const std::string &response) {
  if (clients.find(client_fd) == clients.end()) {
    return;
  }
  Client *client = clients[client_fd];
  ssize_t bytes_sent = send(client_fd, response.c_str(), response.size(), 0);   // Send the response back to the connected client
                                                                                // client_fd         : file descriptor of the client socket
                                                                                // response.c_str()  : pointer to the character array (C-style string) to send
                                                                                // response.size()   : number of bytes to send from the response string
                                                                                // 0                 : flags (0 = default send behavior)
                                                                                // Return:
                                                                                // > 0  -> number of bytes successfully sent
                                                                                // = 0  -> no bytes were sent (edge case, rare)
                                                                                // < 0  -> error occurred (check errno for the reason)
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
                                                // Set the file descriptor to non-blocking mode
                                                // This means I/O operations like read(), write(), accept(), etc. will return immediately
                                                // instead of waiting (blocking) if they can't proceed right away.
  if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
    perror("fcntl O_NONBLOCK failed");
    return false;
  }
  return true;
}

int Server::findClientByNickname(const std::string &nickname) {
  for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it) {
    if (it->second->nickname == nickname) {
      return it->first;
    }
  }
  return -1;  
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
  std::cout << YELLOW << "Server log: " << RESET
            << std::endl;
  std::cout << "═══════════════════════════════════════════════════════════════"
               "════════ "
            << std::endl;
};
