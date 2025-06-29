/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:54:08 by nige42            #+#    #+#             */
/*   Updated: 2025/06/21 07:32:01 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <sys/epoll.h>

#include "Channel.hpp"
#include "Client.hpp"
#include "SigHandler.hpp"
/*#include "user.hpp"*/

#define RED "\033[31m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define GREY "\033[30m"
#define YELLOW "\033[33m"
#define LIGHTBLUE "\033[36m"
#define PINK "\033[35m"
#define RESET "\033[0m"

#define BUFFER 1024

struct Command {
  std::string prefix;
  std::string name;
  std::vector<std::string> params;
  std::string trailing;
};

class User;
class Channel;
class Client;

class Server {

public:
  Server(int port, std::string password);
  ~Server();

  // nigel
  void createServer(void);
  void userLoopCheck();
  void putServerBanner(void);
  std::string putClientBanner(void);

private:
  int port_;
  int socket_;
  std::string password_;
  int lastClientToWrite_;
  std::map<int, Client *> clients;
  std::map<std::string, Channel *> channels;

  void acceptNewClient(int epfd);
  void handleClientMessage(int client_fd);
  void disconnectClient(int client_fd);

  Command parseCommand(const std::string &input);
  std::string buildMessage(const std::string &prefix, const std::string &name,
                           const std::vector<std::string> &params,
                           const std::string &trailing);
  void sendResponse(int client_fd, const std::string &response);
  void broadcastResponse(const std::string &message,
                         const std::vector<int> &client_fds);

  // commands
  void handlePing(int client_fd, const Command &command);
  void handlePong(int client_fd, const Command &command);
  void handleNick(int client_fd, const Command &command);
  void handleJoin(int client_fd, const Command &command);
  void handlePart(int client_fd, const Command &command);
  void handlePrivmsg(int client_fd, const Command &command);
  void handleQuit(int client_fd, const Command &command);
  void handlePass(int client_fd, const Command &command);
  void handleUser(int client_fd, const Command &command);
  void handleMode(int client_fd, const Command &command);
  void handleTopic(int client_fd, const Command &command);
  void handleKick(int client_fd, const Command &command);
  void handleInvite(int client_fd, const Command &command);
  void handleCap(int client_fd, const Command &command);
  void handleUnknownCommand(int client_fd, const Command &command);

  // Utility functions
  std::string getCurrentTime();
  bool setNonBlocking(int fd);
  bool isAvailableNickname(const std::string &nickname);
  bool isValidNickname(const std::string &nickname);
  bool isChannelExists(const std::string &channel_name);
  std::set<int> getChannelsClientList(int client_fd);
  void sendWelcomeMessage(int client_fd);
  void sendWelcomeJoinMessage(int client_fd, const std::string &channel_name);
  void handleChannelMessage(int client_fd, const std::string &target, const std::string &message);
  void handlePrivateMessage(int client_fd, const std::string &target, const std::string &message);
  int findClientByNickname(const std::string &nickname);
  void viewTopic(int client_fd, const std::string &channel_name);
  void changeTopic(int client_fd, const std::string &channel_name, const std::string &new_topic);
  void listInvites(int client_fd);
  void sendInvite(int client_fd, const std::string &target_nickname, const std::string &channel_name);
};
