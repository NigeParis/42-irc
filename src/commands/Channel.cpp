#include "../../include/Channel.hpp"

Channel::Channel(const std::string &channel_name, const std::string &creation_time)
    : name(channel_name), topicSetBy(""), topicSetTime(""), isInviteOnly(false), topicRestricted(false),
      isPrivate(false), userLimit(-1), creationTime(creation_time) {}

void Channel::setTopic(const std::string &new_topic) { topic = new_topic; }
std::string Channel::getTopic() const { return topic; }

void Channel::addUser(Client *user) {
  users[user->socket_fd] = user;
  if (operators.empty()) {
    operators.insert(user->socket_fd);
  }
}

void Channel::removeUser(int user_fd) {
  users.erase(user_fd);
  operators.erase(user_fd);
}

bool Channel::hasUser(int user_fd) const {
  return users.find(user_fd) != users.end();
}

std::vector<int> Channel::getClientFds() {
  std::vector<int> client_fds;
  for (std::map<int, Client *>::iterator it = users.begin(); it != users.end();
       ++it) {
    client_fds.push_back(it->first);
  }
  return client_fds;
}

bool Channel::isWhiteListed(int user_fd) const {
  if (!isInviteOnly)
    return true;

  if (invitedUsers.find(user_fd) != invitedUsers.end())
    return true;

  return false;
}

bool Channel::verifyPassword(const std::string &password) const {
  if (!isPrivate)
    return true;

  return this->password == password;
}

bool Channel::isOperator(int client_fd) const {
  return operators.find(client_fd) != operators.end();
}

void Channel::removeUserFromOperators(int user_fd) {
  operators.erase(user_fd);
}

void Channel::addUserToOperators(int user_fd) {
  operators.insert(user_fd);
}

void Channel::removeUserFromInvitedUsers(int user_fd) {
  invitedUsers.erase(user_fd);
}

void Channel::addUserToInvitedUsers(int user_fd) {
  invitedUsers.insert(user_fd);
}

bool Channel::isInvited(int user_fd) const {
  return invitedUsers.find(user_fd) != invitedUsers.end();
}