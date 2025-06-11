/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 11:07:49 by nige42            #+#    #+#             */
/*   Updated: 2025/06/11 17:25:54 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/user.hpp"

User::User(int fd, short events, short revents): nickName_("guest"), userName_("default") {

this->socketFd_ = fd;

this->user_pollfd.fd = fd;
this->user_pollfd.events = events;
this->user_pollfd.revents = revents;


}

User::~User(void) {
    
};

void User::setUserFd(int fd) {
    this->socketFd_ = fd;
};


int User::getUserFd(void) {
    return (this->socketFd_);
};



void User::setNickName(std::string &name) {

    std::stringstream ss;
    ss << "#" << this->socketFd_;
    
    if (!name.empty()) {
        this->nickName_ = name + ss.str();
    } else {
        this->nickName_ = ss.str();
    }
}


std::string User::getNickName(void) {
    return (this->nickName_);
};
