/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nige42 <nige42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 11:07:49 by nige42            #+#    #+#             */
/*   Updated: 2025/06/08 17:33:15 by nige42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/user.hpp"

User::User(int fd, short events, short revents) {

(void) fd;
(void) events;
(void) revents;
this->socketFd_ = fd;

//     this->user_pollfd.fd = fd;
//     this->user_pollfd.events = events;
//     this->user_pollfd.revents = revents;
//      ?????

}

User::~User(void) {
    
};

void User::setUserFd(int fd) {
    this->socketFd_ = fd;
};


int User::getUserFd(void) {
    return (this->socketFd_);
};

