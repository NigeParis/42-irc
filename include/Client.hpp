#pragma once

#include "Utils.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

class Client {

public:
  int socket_fd;
  std::string nickname;
  std::string username;
  std::string realname;
  bool is_authenticated;
  std::string buffer;

  Client(int fd);
  ~Client();
};
