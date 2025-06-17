/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 15:01:27 by nige42            #+#    #+#             */
/*   Updated: 2025/06/17 06:53:04 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <poll.h>
#include <vector> 
#include <sstream>
#include "../include/server.hpp"

class User {

    public:

        User (int fd, short events, short revents);
        ~User(void);
        
        void setUserFd(int fd);
        int getUserFd(void);
        void setNickName(std::string &name);
        std::string getNickName(void);
        void setUserName(std::string &name);
        std::string getUserName(void);
        bool getUserPassword(void);
        void setUserPassword(bool status);

        struct pollfd user_pollfd;
        
    private:

        int socketFd_;
        std::string nickName_;
        std::string userName_;
        bool userPassword_;
    
};