/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:54:08 by nige42            #+#    #+#             */
/*   Updated: 2025/06/16 20:31:04 by nrobinso         ###   ########.fr       */
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
#include <signal.h>
#include <string>

#include <sys/epoll.h>
#include <cstdio>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>

#include "user.hpp"
#include "SigHandler.hpp"
#include "parsing.hpp"

#define RED "\033[31m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define GREY "\033[30m"
#define YELLOW "\033[33m"
#define LIGHTBLUE "\033[36m"
#define PINK "\033[35m"
#define RESET "\033[0m"


#define BUFFER 1024


enum ServerCommands {

    NOTICE,
    EXIT    
};



class User;

class Server {

    public:
    
        Server(int port, std::string password);
        ~Server();        size_t getUsersSize(void);

        void createServer(void);
        void makeUser(void);        
        void readMessage(User& user);
        void sendMessage(User &user, std::string message);
        void userLoopCheck(void);
        User* findUserByFd(int fd);
        void getClientMessage (int client_fd);
        void putServerBanner(void);
        void addNewClient(epoll_event &user_ev, int epfd);
        std::string  putClientBanner(void);


        void sendMessageAll(std::string message);
        
        void setWriterFd(int fd);
        int getWriterFd(void);
        
      
        void timeStamp(void);




        void pong(int socket, std::string message);
        int cap(int socket, std::string message);


        //parsing commands
        void clientInputCommand (int socket, std::string &inputClient); 
        

        
        std::vector<User*> users_;

        
        private:
        
        int port_;
        int socket_;
        std::string password_;
        int lastClientToWrite_;
        

    
};
