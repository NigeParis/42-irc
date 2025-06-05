/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nige42 <nige42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:54:04 by nige42            #+#    #+#             */
/*   Updated: 2025/06/05 10:32:59 by nige42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"


Server::Server(int port, std::string password) {
    
    (void)port;
    (void)password;
    std::cout << "constructor" << " port: "<< port 
    << " PW: " << password << std::endl;
    
    
};

Server::~Server() {

    std::cout << "destructor" << std::endl;
    
};


void Server::createServer(void) {

    struct sockaddr_in addressServer;
  
    this->socket_ = socket(AF_INET, SOCK_STREAM, 0);
    bind(this->socket_, (struct sockaddr*)&addressServer, sizeof(addressServer));
    listen(this->socket_, 10); 

    std::cout << "server listening on: " << port_ << std::endl;
    
};
