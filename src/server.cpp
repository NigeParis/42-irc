/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:54:04 by nige42            #+#    #+#             */
/*   Updated: 2025/06/26 14:04:31 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

Server::Server(int port, std::string password) : port_(port), password_(password), lastClientToWrite_(0) {
    //std::cout << "constructor" << " port: "<< port << " PW: " << password << std::endl;
};

Server::~Server() {

    std::cout << BLUE << "[DEBUG] - destructor Server" << RESET << std::endl;    

    for (size_t i = 0; i < users_.size(); ++i) {
        delete users_[i];
    }
    users_.clear(); 
    
    // for (size_t i = 0; i < channels_.size(); ++i) {
    //     delete channels_[i];
    // }
    
    // for (size_t y = 0; y < this->channels_.size(); ++y) {
    //     for (size_t i = 0; i < this->channels_[y]->channelmembre_.size(); ++i) {
    //         delete this->channels_[y]->channelmembre_[i];
    //     }
    //     for (size_t i = 0; i < this->channels_[y]->channelInvited_.size(); ++i) {
    //         delete this->channels_[y]->channelInvited_[i];
    //     }   
    //     for (size_t i = 0; i < this->channels_[y]->channelBoss_.size(); ++i) {
    //         delete this->channels_[y]->channelBoss_[i];
    //     }       
    // }
    
    for (std::vector<Channel*>::iterator it = channels_.begin(); it != channels_.end(); ++it) {
        delete *it;
    }
    channels_.clear(); 
    
    close(this->socket_);    
};


void Server::createServer(void) {

    struct sockaddr_in addressServer;
    int opt = 1;
        
    this->socket_ = socket(AF_INET, SOCK_STREAM, 0);
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

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    
    int clientFd = accept(socket_, (struct sockaddr *)&client_addr, &client_addr_len);
    if (clientFd == -1) {
        std::cout << "Error: creating client FD" << std::endl;
        SigHandler::sigloop = false;
        return ;
    }
    fcntl(clientFd, F_SETFL, O_NONBLOCK);
    User* user = new User(clientFd, POLLIN, 0); 
    users_.push_back(user);
    std::string name = user->getNickName(); // default name
    user->setNickName(name);    
    Server::timeStamp(); 
    std::cout << BLUE << "[LOGIN]     " << RESET << "<" << GREEN << std::setfill(' ') << std::setw(8) << user->getNickName() << RESET << ">" << " Just arrived " << std::endl;   

}



   


void Server::clientQuits(int fd, User &user) {

    std::vector<User*>::iterator it = std::find(users_.begin(), users_.end(), &user);
    Server::timeStamp(); 
    std::cout << BLUE << "[LOGOUT]    " << RESET << "<" << GREEN << std::setfill(' ') << std::setw(8)  << user.getNickName() << RESET << ">" << " Just left " << std::endl;
    this->lastClientToWrite_ = 0;  
    close(fd);
    delete &user;
    if (it != users_.end()) {
        users_.erase(it);
        Server::timeStamp();
        std::cout << YELLOW << "[CLIENTS]   " << RESET << "<" << std::setfill(' ') << std::setw(7) << YELLOW  << "active"  << RESET << "> " << users_.size() << std::endl;
    }            
};

void Server::readMessage(User &user) {

    std::string buffer(BUFFER, '\0');
    static int checkfd;
    
    int fd = user.getUserFd();
    ssize_t bytes_read = recv(user.getUserFd(), &buffer[0], buffer.size() - 1, 0);
    if ((bytes_read == 0) || SigHandler::sigloop == false) {
        clientQuits(fd, user);
        return ;
    }
    if (bytes_read == -1) {
        std::cout << "error recv()" << std::endl;
        return ;
    }
    if (buffer[0] == '\r')
        return;
    else if (bytes_read != -1) {
        if (bytes_read && lastClientToWrite_ != user.getUserFd() && buffer[0] != '\r') {
            Server::timeStamp(); 
            std::cout << RED << "[MESSAGE]   " << RESET << "<" << GREEN << std::setfill(' ') << std::setw(8) << user.getNickName() << RESET << "> ";
        }
        checkfd = user.getUserFd();
        std::cout << buffer;

        clientInputCommand(user.getUserFd(), buffer);     // commands recieved from client  
        
        
        if (buffer[bytes_read - 2] == '\r')
            this->lastClientToWrite_ = 0;
        else      
            this->lastClientToWrite_ = checkfd;
    }
};


void Server::getClientMessage (int client_fd){

    User* user = findUserByFd(client_fd);
    if (user != NULL) {
        readMessage(*user);       
    } 
};


void Server::addNewClient(epoll_event &user_ev, int epfd) {

    std::string message;
    makeUser();
    if (SigHandler::sigloop == false) 
        return ;

    user_ev.events = EPOLLIN;
    user_ev.data.fd = users_.back()->user_pollfd.fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, users_.back()->user_pollfd.fd, &user_ev);
    Server::timeStamp(); 
    std::cout << YELLOW << "[CLIENTS]   " << RESET << "<" << std::setfill(' ') << std::setw(7) << YELLOW  << "active"  << RESET << "> " << users_.size() << std::endl;

    
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
    }    
    close(epfd);
}


// Helper functions

User* Server::findUserByFd(int fd) {
    for (size_t i = 0; i < users_.size(); i++) {
        if (users_[i]->getUserFd() == fd) {
            return users_[i];
        }
    }
    return NULL;
};



void Server::putServerBanner(void) {

    std::cout << "██████╗███████╗██████╗ ██╗   ██╗███████╗██████╗     ██╗██████╗  ██████╗ " << std::endl;
    std::cout << "█╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗    ██║██╔══██╗██╔════╝ "<< std::endl;
    std::cout << "██████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝    ██║██████╔╝██║" << std::endl;
    std::cout << "════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗    ██║██╔══██╗██║" << std::endl;   
    std::cout << "██████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║    ██║██║  ██║╚██████╗ " << std::endl;
    std::cout << "══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝    ╚═╝╚═╝  ╚═╝ ╚═════╝ " << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════════════ " << std::endl;
    std::cout << "server listening on port: " << this->port_ << std::endl;
    std::cout << "server password: " << this->password_ << std::endl;
    std::cout << std::endl;
    std::cout << YELLOW << "Users connected: " << users_.size() << RESET << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════════════ " << std::endl;

};


std::string  Server::putClientBanner(void) {

    std::stringstream ss;

    ss << "██╗██████╗  ██████╗     ██████╗██╗     ██╗███████╗███╗   ██╗████████╗ \n";
    ss << "██║██╔══██╗██╔════╝    ██╔════╝██║     ██║██╔════╝████╗  ██║╚══██╔══╝ \n";
    ss << "██║██████╔╝██║         ██║     ██║     ██║█████╗  ██╔██╗ ██║   ██║ \n";
    ss << "██║██╔══██╗██║         ██║     ██║     ██║██╔══╝  ██║╚██╗██║   ██║ \n";   
    ss << "██║██║  ██║╚██████╗    ╚██████╗███████╗██║███████╗██║ ╚████║   ██║ \n";
    ss << "══════════════════════════════════════════════════════════════════ \n";

    return (ss.str());
};

   
void Server::sendMessage(User &user, std::string message) {

    if (user.getUserFd() > -1 && !message.empty())
        sendCommand(user.getUserFd(), message);
};


void Server::sendMessageAll(std::string message) {

    for (size_t i = 0;  i < users_.size(); i++)
        sendCommand(users_[i]->getUserFd(), message);
};


void Server::timeStamp(void) {

    std::time_t now = std::time(NULL);
    std::tm *localTime = std::localtime(&now);
    
    std::cout 
    << std::setfill('0') 
    << std::setw(2) << localTime->tm_hour 
    << ":" 
    << std::setw(2) << localTime->tm_min 
    << ":" 
    << std::setw(2) << localTime->tm_sec << " ";
};
