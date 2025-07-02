#include "../include/Client.hpp"

Client::Client(int fd, int guest_number)
    : socket_fd(fd), nickname("Guest" + intToString(guest_number)),
      username("default"), hostname("default"), is_authenticated(false){
        quote = Quote();
      }

Client::~Client() { close(socket_fd); }
