/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 06:53:08 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/21 15:21:26 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"


Channel::Channel(std::string name): channelName_(name) {

    //std::cout << "Channel constructor" << std::endl;
    this->I_ = false;  // set/remove Invite only channel
    this->T_ = false;  // set/remove topic command to channel operators
    this->K_ = false;  // set/remove channel key password
    this->O_ = false;  // give/take channel operator privilege
    this->L_ = false;  // set/remove user limit to channel
    this->channelPassword_ = "";
    this->channelTopic_ = "Default Unknown Topic";
};

Channel::~Channel(void) {
    std::cout << BLUE << "[DEBUG - destructor Channel]" << RESET << std::endl;    

};

std::string Channel::getChannelName(void) {
    return (this->channelName_);
};

void Channel::setChannelName(std::string name) {

    if (name.empty())
        return ;
    this->channelName_ = name;
};

std::string Channel::getChannelPassword(void) {
    return (this->channelPassword_);
};

void Channel::setChannelPassword(std::string password) {
    if (password.empty())
        return ;
};

std::string Channel::getChannelTopic(void) { 
    return (this->channelTopic_);
};

void Channel::setChannelTopic(std::string topic) {

    if (topic.empty())
        return ;
    this->channelTopic_ = topic;
};




