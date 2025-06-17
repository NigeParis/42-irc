/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 14:04:16 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/17 11:07:16 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parsing.hpp"


std::string extractWord(std::string &input, std::string strFind) {

    std::string string;
    
    if (input.empty())
        return ("");
    std::size_t pos = input.find(strFind);
    if (pos == input.npos)
        return ("");
    std::size_t pStart = input.find(" ", pos) + 1;
    std::size_t pEnd = input.find('\r', pStart);
    string = input.substr(pStart, pEnd - pStart);
    return (string);
}


void Server::initClientsNames(int clientFd, std::string &inputClient) {

    static bool initNames = false;

    User *user = findUserByFd(clientFd);
    std::string nickName = extractWord(inputClient, "NICK ");

    if (initNames == false)
        user->setNickName(nickName);
    initNames = true;

}



void Server::clientInputCommand(int clientFd, std::string &inputClient) {

    if (inputClient.empty()) return;

    
    User *user = findUserByFd(clientFd);
    size_t start = inputClient.find_first_not_of(" \t");
    size_t end = inputClient.find_first_of(" \t");
    std::string result = inputClient.substr(start, end - start);    
    std::string message = inputClient.substr(end - start + 1, inputClient.size()); 
    initClientsNames(clientFd, inputClient);
    std::string nickName = extractWord(inputClient, "NICK ");

    
    std::string welcomeMessage = "001 " + user->getNickName() + " :Welcome " + user->getNickName() + "\n" +  putClientBanner() + "\r\n";

    if (result == "PING") {
        pong(clientFd, message);        
    }    
    if ((result == "CAP") && inputClient.substr(4,2) == "LS") {
        cap(clientFd, "CAP * LS :\r\n");
        send(clientFd, welcomeMessage.c_str(), welcomeMessage.size(), 0); 
    }
    
    

        
    if (result == "NICK") {
        nick(clientFd, nickName);        
    }  
}; 



void Server::pong(int clientFd, std::string input) {

    std::string buildMessage;
    
    buildMessage = "PONG ";
    buildMessage += input;
    buildMessage += "\r\n";
    send(clientFd, buildMessage.c_str(), buildMessage.size(), 0);
}


int Server::cap(int clientFd, std::string input) {

    std::string buildMessage;
    
    buildMessage = input;
    if ((send(clientFd, buildMessage.c_str(), buildMessage.size(), 0)) > 0) {
        return (0);
    }
    return (1);
};


int Server::nick(int clientFd, std::string input) {

    (void)clientFd;
    std::string message;
    User *user = findUserByFd(clientFd);
    
    message = ":" + user->getNickName();
    message += " NICK " + input;
    message += "\r\n";
    Server::timeStamp(); 
    std::cout << RED << "[MESSAGE]   " << RESET << "<" << GREEN << user->getNickName() << RESET << "> ";
    std::cout << "Changed nickname to " << "<" << GREEN << input << RESET << ">" << std::endl;
    user->setNickName(input);
    send(clientFd, message.c_str(), message.size(), 0);
    return (1);
};
