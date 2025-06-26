#include "../include/Channel.hpp"

Channel::Channel(const std::string &channel_name)
    : name(channel_name), isInviteOnly(false), topicRestricted(false),
      isSecret(false), userLimit(-1) {}

void Channel::setTopic(const std::string &new_topic) { topic = new_topic; }
void Channel::addUser(Client &user) {
  users[user.socket_fd] = &user;
  if (operators.empty()) {
    operators.insert(user.socket_fd);
  }
}

void Channel::removeUser(int user_fd) {
  std::map<int, Client *>::iterator it = users.find(user_fd);
  if (users.find(user_fd) != users.end()) {
    users.erase(it);
  }
  if (operators.find(user_fd) != operators.end()) {
    operators.erase(user_fd);
  }
}

bool Channel::hasUser(int user_fd) const {
  return users.find(user_fd) != users.end();
}
