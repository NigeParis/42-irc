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

#include "SigHandler.hpp"
#include "parsing.hpp"
#include "user.hpp"

#define RED "\033[31m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define GREY "\033[30m"
#define YELLOW "\033[33m"
#define LIGHTBLUE "\033[36m"
#define PINK "\033[35m"
#define RESET "\033[0m"

#define BUFFER 1024

class User;
class Channel;

class Server {

public:
  Server(int port, std::string password);
  ~Server();
  size_t getUsersSize(void);

  void createServer(void);
  void makeUser(void);
  void readMessage(User &user);
  void sendMessage(User &user, std::string message);
  void userLoopCheck(void);
  User *findUserByFd(int fd);
  void getClientMessage(int client_fd);
  void putServerBanner(void);
  void addNewClient(epoll_event &user_ev, int epfd);
  std::string putClientBanner(void);

  void sendMessageAll(std::string message);

  void timeStamp(void);

  // parsing commands
  void clientInputCommand(int clientFD, std::string &inputClient);
  void pong(int clientFd, std::string input);
  void clientQuits(int fd, User &user);
  std::string trimUserName(std::string &userName);
  std::string extractClientData(std::string &input, std::string strFind);
  int checkLeadingHash(int clientFd, std::string &input);
  void putErrorMessage(int clientFd, std::string &input, std::string errorMsg,
                       int code);
  std::string extractRealName(std::string &realName);
  std::string putWelcomeMessage(User *user);

  void cap(int clientFd, std::string &inputClient, User *user);
  int sendCommand(int clientFd, std::string input);

  void nick(int clientFd, std::string &inputClient, User *user);
  int nickCommand(int clientFd, std::string input);

  void join(int clientFd, std::string &inputClient, User *user);

  int checkForSpaces(int clientFd, std::string &input);

  int initClientsNames(int clientFd, std::string &inputClient, User &user);

  std::vector<User *> users_;
  std::vector<Channel *> channels_;

private:
  int port_;
  int socket_;
  std::string password_;
  int lastClientToWrite_;
};
