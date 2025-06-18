/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 11:07:49 by nige42            #+#    #+#             */
/*   Updated: 2025/06/18 14:56:52 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/user.hpp"

User::User(int fd, short events, short revents): nickName_("guest"), userName_("default"), userPassWord_(""), validPassword_(false) {

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

    if (name != "guest") {
        this->nickName_ = name;
        return ;
    }
    std::stringstream ss;
    ss << "_" << this->socketFd_;
    
    if (!name.empty()) {
        this->nickName_ = name + ss.str();
    } else {
        this->nickName_ = ss.str();
    }
}






std::string User::getNickName(void) {
    return (this->nickName_);
};


void User::setUserName(std::string &name) {
    if(name.empty())
        return ;
    this->userName_ = name;
};

std::string User::getUserName(void) {
    return (this->userName_);
};



bool User::getValidPassword(void) {
    return (this->validPassword_);  
};

void User::setValidPassword(bool status) {
    this->validPassword_ = status;
};





std::string User::getUserPassword(void) {
    return (this->userPassWord_);  
};

void User::setUserPassword(std::string password) {
    this->userPassWord_ = password;
};
