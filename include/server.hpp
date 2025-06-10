/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:54:08 by nige42            #+#    #+#             */
/*   Updated: 2025/06/10 10:26:42 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <sstream>

#include "user.hpp"

#define BUFFER 1024

class User;

class Server {

    public:
    
        Server(int port, std::string password);
        ~Server();
        void createServer(void);
        void makeUser(void);        
        void readMessage(User& user);
        void sendMessage(User &user, std::string message);
        void userLoopCheck(void);

                    
        std::vector<User*> users_;

    private:
    
        int port_;
        int socket_;
        std::string password_;

    
};
