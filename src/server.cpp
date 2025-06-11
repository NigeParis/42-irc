/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:54:04 by nige42            #+#    #+#             */
/*   Updated: 2025/06/11 14:16:34 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

// helper functions


Server::Server(int port, std::string password) : port_(port), password_(password){
    //std::cout << "constructor" << " port: "<< port << " PW: " << password << std::endl;
};

Server::~Server() {

    std::cout << "destructor" << std::endl;
    for (size_t i = 0; i < users_.size(); ++i) {
        delete users_[i];
    }
    users_.clear();
    close(this->socket_);

    
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

    putServerBanner();
};



void Server::makeUser(void) {

    std::string name = "guest"; // default name
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    
    int client_fd = accept(socket_, (struct sockaddr *)&client_addr, &client_addr_len);
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    User* user = new User(client_fd, POLLIN, 0);
    
    users_.push_back(user);

    user->setNickName(name);
    
    sendMessage(*user, connectMessage(user));

   
}



void Server::readMessage(User &user) {

    std::string buffer(BUFFER, '\0');
    int fd = user.getUserFd();
    ssize_t bytes_read = recv(user.getUserFd(), &buffer[0], buffer.size() - 1, 0);
        
    if (bytes_read == 0) {
        std::vector<User*>::iterator it = std::find(users_.begin(), users_.end(), &user);  
        delete &user;
        if (it != users_.end()) {
            users_.erase(it);
        close(fd);
        return ;
        }            
    }
    if (bytes_read != -1 && buffer[0] != '\r') {
        std::cout << RED << "Received message: "  << RESET;
        std::cout << buffer;
    }
};



void Server::sendMessage(User &user, std::string message) {

    if (user.getUserFd() > -1 && !message.empty())
        send(user.getUserFd(), message.c_str(), message.size(), 0);

    
}


void Server::getClientMessage (int client_fd){

    User* user = findUserByFd(client_fd);
    if (user != NULL) {
        readMessage(*user);       
    } 
};






void Server::userLoopCheck() {
    // std::cout << "userLoopCheck()" << std::endl;
    epoll_event events[BUFFER];
    struct epoll_event ev;
    struct epoll_event user_ev;
    
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create failed");
        return;
    }
    
    ev.events = EPOLLIN;
    ev.data.fd = this->socket_;
    epoll_ctl(epfd, EPOLL_CTL_ADD, this->socket_, &ev);

    while (SigHandler::sigloop) {
        //std::cout << "sig value" << SigHandler::sigloop << std::endl;
        if (!SigHandler::sigloop) break;

        int num_events = epoll_wait(epfd, events, 10, 1000);
        if (num_events < 0) {
            if (!SigHandler::sigloop) break;
            perror("epoll_wait failed");
            break;
        }
        //std::cout << "sig value" << SigHandler::sigloop << std::endl;
        if (!SigHandler::sigloop) break;
    
        for (int i = 0; i < num_events; ++i) {
            int fd = events[i].data.fd;

            if (fd == this->socket_ && (events[i].events & EPOLLIN)) {
                std::cout << "New client detected! Calling makeUser()" << std::endl;
                makeUser();

                user_ev.events = EPOLLIN;
                user_ev.data.fd = users_.back()->user_pollfd.fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, users_.back()->user_pollfd.fd, &user_ev);
                std::cout << "Users connected: " << users_.size() << std::endl;

            }
            else if (events[i].events & EPOLLIN) {  
                if ((events[i].events & (EPOLLERR | EPOLLHUP)) == 0)           
                    getClientMessage(fd);
            }

        }
    }

    close(epfd);
}






// Helper functions


std::string Server::connectMessage(User *user) {
    std::stringstream ss;
    std::string message = "Welcome ";
    ss << user->getUserFd();
    message += user->getNickName();
    //message += "#" + ss.str();
    message += "\n";
    return (message);
}


User* Server::findUserByFd(int fd) {
    for (size_t i = 0; i < users_.size(); i++) {
        if (users_[i]->getUserFd() == fd) {
            return users_[i];
        }
    }
    return NULL;
}


void Server::putServerBanner(void) {

    std::cout << "███████╗███████╗██████╗██╗   ██╗███████╗██████╗     ██╗██████╗  ██████╗ " << std::endl;
    std::cout << "█╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗    ██║██╔══██╗██╔════╝ "<< std::endl;
    std::cout << "██████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝    ██║██████╔╝██║" << std::endl;
    std::cout << "════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗    ██║██╔══██╗██║" << std::endl;   
    std::cout << "██████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║    ██║██║  ██║╚██████╗ " << std::endl;
    std::cout << "══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝    ╚═╝╚═╝  ╚═╝ ╚═════╝ " << std::endl << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════════════ " << std::endl;
    std::cout << "server listening on port: " << this->port_ << std::endl;
};
