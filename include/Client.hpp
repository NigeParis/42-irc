#pragma once

#include "Utils.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include "Quote.hpp"

class Quote;

class Client {

public:
  int socket_fd;
  std::string nickname;
  std::string username;
  std::string hostname;
  bool is_authenticated;
  std::string buffer;
  Quote quote;

  Client(int fd, int guest_number);
  ~Client();
};
