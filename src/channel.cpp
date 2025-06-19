/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 06:53:08 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/19 07:32:47 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"


Channel::Channel(std::string name): channelName_(name) {

    //std::cout << "Channel constructor" << std::endl;
    this->I_ = false;
    this->K_ = false;
    this->L_ = false;
    this->O_ = false;
    this->T_ = false;
    this->channelPassword_ = "";
    this->channelTopic_ = "";
};

Channel::~Channel(void) {
    //std::cout << "Channel Destructor" << std::endl;
    
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




