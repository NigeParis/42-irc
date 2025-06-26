/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsingTools.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 09:38:41 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/26 13:52:38 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

int Server::checkForSpaces(int clientFd, std::string &input) {

    User *user = findUserByFd(clientFd);
    
    // if (input.empty()) {
    //     return (1);
    // }
    for (std::string::iterator it = input.begin(); it < input.end(); it++) {
        if (*it == ' ') {
            std::string errorMessage = "Error " + user->getNickName() + " " + input + " :Erroneus nickname";
            Server::timeStamp(); 
            std::cout << RED << "[MESSAGE]   " << RESET << "<" << GREEN << std::setfill(' ') << std::setw(8) << user->getNickName() << RESET << "> ";
            std::cout << errorMessage << std::endl;
            errorMessage += "\r\n";    //commande ended signal
            sendCommand(clientFd, errorMessage);
            return (1); 
        }      
    }
    return (0);
};


std::string extractTopic(std::string &topic) {

    std::string newTopic;
    int start = topic.find(":") + 1;
    int end = topic.size();
    newTopic = topic.substr(start, end);
    std::cout << BLUE << "[DEBUG] - extractChannel: " << "'" << newTopic << "'" << RESET << std::endl;
    return (newTopic);
}

std::string extractChannel(std::string &topic) {

    std::string channel;
    int start = 1;
    int end = topic.find(" ") - 1;
    channel = topic.substr(start, end);
    std::cout << BLUE << "[DEBUG] - extractChannel: " << "'" << channel << "'" << RESET << std::endl;
    return (channel);
}




// Checks for leading #nick - interdiction error (432)
//ERR_ERRONEUSNICKNAME (432)

int Server::checkLeadingHash(int clientFd, std::string &input) {
    
    if ((*input.begin() == '#') || (*input.begin() == ':')) {
        putErrorMessage(clientFd, input, " :Erroneus nickname", 432);
        return(1);
    }
    return (0);
};

std::string Server::trimUserName(std::string &userName) {
    
    std::string trimmedString;
    
    size_t pos = 0;
    size_t end = userName.find(" ");
    trimmedString = userName.substr(pos, end);
    return(trimmedString);   
}


std::string Server::extractClientData(std::string &input, std::string strFind) {
    
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


std::string Server::extractRealName(std::string &realName) {
    
    std::string newString;
    int pos;
    int end = realName.size();
    
    if (realName.empty())
    return ("");
    pos = realName.find(":") + 1;
    newString = realName.substr(pos, end);
    return (newString);  
};

