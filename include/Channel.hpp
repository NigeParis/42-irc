#pragma once

#include "Client.hpp"
#include <map>
#include <set>
#include <string>

class Channel {
private:
  std::string topic;
  std::map<int, Client *> users;
  std::set<int> operators;
  std::set<int> invitedUsers;

public:
  std::string name;
  std::string password;
  std::string topicSetBy;
  std::string topicSetTime;
  bool isInviteOnly;
  bool topicRestricted;
  bool isPrivate;
  int userLimit;
  std::string creationTime;

  Channel(const std::string &channel_name, const std::string &creation_time);
  void setTopic(const std::string &new_topic);
  void addUser(Client *user);
  void removeUser(int user_fd);
  bool hasUser(int user_fd) const;
  bool isWhiteListed(int user_fd) const;
  bool verifyPassword(const std::string &password) const;
  void removeUserFromOperators(int user_fd);
  void addUserToOperators(int user_fd);
  void removeUserFromInvitedUsers(int user_fd);
  void addUserToInvitedUsers(int user_fd);
  bool isInvited(int user_fd) const;

  std::vector<int> getClientFds();
  // Getters
  std::string getTopic() const;
  bool isOperator(int client_fd) const;

  
};
