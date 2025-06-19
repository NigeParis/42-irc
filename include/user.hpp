/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 15:01:27 by nige42            #+#    #+#             */
/*   Updated: 2025/06/19 07:23:55 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <poll.h>
#include <vector> 
#include <sstream>
#include "../include/server.hpp"
#include "../include/channel.hpp"

class Channel;

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
        void setRealName(std::string &name);
        std::string getRealName(void);
        std::string getUserPassword(void);
        void setUserPassword(std::string password);
        void setValidPassword(bool status);
        bool getValidPassword(void);


        struct pollfd user_pollfd;
        Channel *channel;
        
    private:

        int socketFd_;
        std::string nickName_;
        std::string userName_;
        std::string userPassWord_;
        std::string realName_;
        bool validPassword_;
    
};