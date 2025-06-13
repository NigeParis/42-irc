/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:54:04 by nige42            #+#    #+#             */
/*   Updated: 2025/06/13 10:01:32 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

Server::Server(int port, std::string password) : port_(port), password_(password), lastWritersfd_(0) {
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
    makeServerStdinNonBlocking(); // Set stdin to non-blocking mode
    
};


void Server::makeUser(void) {

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    
    int client_fd = accept(socket_, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1) {
        std::cout << "Error: creating client FD" << std::endl;
        SigHandler::sigloop = false;
        return ;
    }
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    User* user = new User(client_fd, POLLIN, 0);    
    users_.push_back(user);
    std::string name = user->getNickName(); // default name
    user->setNickName(name);
    sendMessage(*user, connectMessage(user));
    std::cout << BLUE << "user: " << user->getNickName() << " enter dans IRC" << RESET << std::endl;

   
}


void Server::readMessage(User &user) {

    std::string buffer(BUFFER, '\0');
    static int checkfd;
    
    int fd = user.getUserFd();
    ssize_t bytes_read = recv(user.getUserFd(), &buffer[0], buffer.size() - 1, 0);
    if (bytes_read == 0) {
        std::vector<User*>::iterator it = std::find(users_.begin(), users_.end(), &user);
        std::cout << BLUE << "user: " << user.getNickName() << " left IRC" << RESET << std::endl;
        this->lastWritersfd_ = 0;  
        close(fd);
        delete &user;
        if (it != users_.end()) {
            users_.erase(it);
            std::cout << GREEN << "Users connected: " << users_.size() << RESET << std::endl;
            return ;
        }            
    }
    if (bytes_read == -1) {
        std::cout << "error recv()" << std::endl;
        return ;
    }
    else if (buffer[0] == '\r')
        return;
    else if (bytes_read != -1) {
        if (bytes_read && lastWritersfd_ != user.getUserFd() && buffer[0] != '\r') {
            std::cout << RED << "Received message: "  << RESET << "[" << user.getNickName() << "]: ";
        }
        checkfd = user.getUserFd();
        std::cout << buffer; 
        if (buffer[bytes_read - 2] == '\r')
            this->lastWritersfd_ = 0;
        else      
            this->lastWritersfd_ = checkfd;
    }
};


void Server::getClientMessage (int client_fd){

    User* user = findUserByFd(client_fd);
    if (user != NULL) {
        readMessage(*user);       
    } 
};

void Server::addNewClient(epoll_event &user_ev, int epfd) {

        //std::cout << "New client detected! Calling makeUser()" << std::endl;
        makeUser();
        if (SigHandler::sigloop == false)
            return ;
        user_ev.events = EPOLLIN;
        user_ev.data.fd = users_.back()->user_pollfd.fd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, users_.back()->user_pollfd.fd, &user_ev);
        std::cout << YELLOW << "Users connected: " << users_.size() << RESET << std::endl;
};





void Server::userLoopCheck() {


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
        if (!SigHandler::sigloop) break;

        int num_events = epoll_wait(epfd, events, 10, 1000);
        if (num_events < 0) {
            if (!SigHandler::sigloop) break;
            perror("epoll_wait failed");
            break;
        }
        for (int i = 0; i < num_events; ++i) {
            int fd = events[i].data.fd;
            if (fd == this->socket_ && (events[i].events & EPOLLIN)) {
                addNewClient(user_ev, epfd);
            } 
            else if (events[i].events & EPOLLIN) {  
                if ((events[i].events & (EPOLLERR | EPOLLHUP)) == 0) {           
                    getClientMessage(fd);
                }
            }
        } 

        runServerCommands();
    }
    close(epfd);
}


// Helper functions

std::string Server::connectMessage(User *user) {
    std::stringstream ss; 
    std::string message =  putClientBanner() + " Welcome ";
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

    std::cout << " ███████╗███████╗██████╗██╗   ██╗███████╗██████╗     ██╗██████╗  ██████╗ " << std::endl;
    std::cout << " █╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗    ██║██╔══██╗██╔════╝ "<< std::endl;
    std::cout << " ██████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝    ██║██████╔╝██║" << std::endl;
    std::cout << " ════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗    ██║██╔══██╗██║" << std::endl;   
    std::cout << " ██████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║    ██║██║  ██║╚██████╗ " << std::endl;
    std::cout << " ══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝    ╚═╝╚═╝  ╚═╝ ╚═════╝ " << std::endl;
    std::cout << " ═══════════════════════════════════════════════════════════════════════ " << std::endl;
    std::cout << "server listening on port: " << this->port_ << std::endl;
    std::cout << "server password: " << this->password_ << std::endl;
    std::cout << YELLOW << "Users connected: " << users_.size() << RESET << std::endl;

};


std::string  Server::putClientBanner(void) {

    std::stringstream ss;

    ss << " ██╗██████╗  ██████╗     ██████╗██╗     ██╗███████╗███╗   ██╗████████╗ \n";
    ss << " ██║██╔══██╗██╔════╝    ██╔════╝██║     ██║██╔════╝████╗  ██║╚══██╔══╝ \n";
    ss << " ██║██████╔╝██║         ██║     ██║     ██║█████╗  ██╔██╗ ██║   ██║ \n";
    ss << " ██║██╔══██╗██║         ██║     ██║     ██║██╔══╝  ██║╚██╗██║   ██║ \n";   
    ss << " ██║██║  ██║╚██████╗    ╚██████╗███████╗██║███████╗██║ ╚████║   ██║ \n";
    ss << " ══════════════════════════════════════════════════════════════════ \n ";
    ss << "connected on port: " << this->port_ << "\n";

    return (ss.str());
};




void Server::setWriterFd(int fd) {
    this->lastWritersfd_ = fd;    
};

int Server::getWriterFd(void) {
    return (this->lastWritersfd_);
};
   
   
void Server::sendMessage(User &user, std::string message) {

    if (user.getUserFd() > -1 && !message.empty())
        send(user.getUserFd(), message.c_str(), message.size(), 0);
}


void Server::sendMessageAll(std::string message) {

    for (size_t i = 0;  i < users_.size(); i++)
        send(users_[i]->getUserFd(), message.c_str(), message.size(), 0);
}


