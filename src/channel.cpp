/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 06:53:08 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/19 07:16:41 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"


Channel::Channel(std::string name): channelName_(name) {

    this->I_ = false;
    this->K_ = false;
    this->L_ = false;
    this->O_ = false;
    this->T_ = false;
    this->channelPassword_ = "";
    this->channelTopic_ = "";
};

Channel::~Channel(void) {

    
};



std::string Channel::getChannelName(void) {
    return (this->channelName_);
};

void Channel::setChannelName(std::string name) {

    if (name.empty())
        return ;
    this->channelName_ = name;
};








