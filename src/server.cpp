/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:54:04 by nige42            #+#    #+#             */
/*   Updated: 2025/06/10 10:00:14 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"


Server::Server(int port, std::string password) : port_(port), password_(password){

    std::cout << "constructor" << " port: "<< port 
    << " PW: " << password << std::endl;
    
    
};

Server::~Server() {

    std::cout << "destructor" << std::endl;
    
};


void Server::createServer(void) {

    struct sockaddr_in addressServer;
        
    this->socket_ = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(this->socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


    memset(&addressServer, 0, sizeof(addressServer));
    addressServer.sin_family = AF_INET;
    addressServer.sin_addr.s_addr = INADDR_ANY;
    addressServer.sin_port = htons(this->port_);


    bind(this->socket_, (struct sockaddr*)&addressServer, sizeof(addressServer));
    listen(this->socket_, 10); 

    std::cout << "server listening on: " << this->port_ << std::endl;


    
    
  

};


void Server::makeUser(void) {


    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    
    int client_fd = accept(socket_, (struct sockaddr *)&client_addr, &client_addr_len);
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    std::cout << "Client connected" << std::endl;
    User* user = new User(client_fd, POLLIN, 0);
  
    users_.push_back(user);
    
}



void Server::readMessage(User &user) {

    std::string buffer(BUFFER, '\0');
    std::cout << "user->fd: " << user.getUserFd() << std::endl;
    ssize_t bytes_read = recv(user.getUserFd(), &buffer[0], buffer.size() - 1, 0);
    if (bytes_read != -1) {
        std::cout << "Received message: " << buffer;
    }
    std::cout << "bytes read: " << bytes_read << std::endl;
};



void Server::sendMessage(User &user, std::string message) {

    if (user.getUserFd() > -1 && !message.empty())
        send(user.getUserFd(), message.c_str(), message.size(), 0);

    
}




void Server::userLoopCheck(void) {


    std::cout << "userLoopCheck()" << std::endl;

    while (1) {
        
            for (size_t i = 0; i < users_.size(); i++) {
        
            int trigger = poll(&users_[i]->user_pollfd, users_.size(), 3000);

            if (trigger > 0) {
                if (this->users_[i]->user_pollfd.revents & POLLIN) {
        
                    std::cout << "reading message" << std::endl;
                    for (size_t i = 0; i < this->users_.size(); i++) {
                        readMessage(*this->users_[i]);
                    }
                }
           }
    }
        // to test the send function to terminals
        //sendMessage(*this->users_[0], "Coucou zero server here \n");
        //sendMessage(*this->users_[1], "Coucou Un server here \n");
        
    }
    
};
