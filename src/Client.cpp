#include "../include/Client.hpp"

Client::Client(int fd)
    : socket_fd(fd), nickname("Guest" + intToString(fd)), username("default"),
      realname("default"), is_authenticated(false) {}

Client::~Client() { close(socket_fd); }
