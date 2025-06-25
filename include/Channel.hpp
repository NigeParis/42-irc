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
  bool isInviteOnly;
  bool topicRestricted;
  bool isSecret;
  int userLimit;

  Channel(const std::string &channel_name);
  void setTopic(const std::string &new_topic);
  void addUser(Client &user);
  void removeUser(int user_fd);
  bool hasUser(int user_fd) const;

  // Getters
  std::string getTopic() const { return topic; }
};
