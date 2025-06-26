#include "../include/Client.hpp"

Client::Client(int fd, int guest_number)
    : socket_fd(fd), nickname("Guest" + intToString(guest_number)),
      username("default"), realname("default"), is_authenticated(false) {}

Client::~Client() { close(socket_fd); }
