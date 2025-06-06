/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nige42 <nige42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:54:04 by nige42            #+#    #+#             */
/*   Updated: 2025/06/06 16:46:32 by nige42           ###   ########.fr       */
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

    users_.push_back(client_fd);
    
}



void Server::readMessage(int client_fd) {

    std::string buffer(BUFFER, '\0');

    ssize_t bytes_read = read(client_fd, &buffer[client_fd], sizeof(buffer) - 1);
    if (bytes_read > 0) {
        std::cout << "Received message: " << buffer;
    }
};



void Server::sendMessage(int client_fd, std::string message) {

    if (client_fd > -1 && !message.empty())
        send(client_fd, message.c_str(), message.size(), 0);

    
}