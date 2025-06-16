/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 14:04:16 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/16 10:33:05 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parsing.hpp"

void Server::clientInputCommand(int socket, std::string &inputClient) {

    if (inputClient.empty()) return;

    size_t start = inputClient.find_first_not_of(" \t");
    size_t end = inputClient.find_first_of(" \t");
    std::string result = inputClient.substr(start, end - start);    
    std::string message = inputClient.substr(end - start + 1, inputClient.size());    

    std::string welcomeMessage = "001 clientNickName :Welcome \n" +  putClientBanner() + "\r\n";

    
    if (result == "PING") {
        pong(socket, message);        
    }    
    if ((result == "CAP") && inputClient.substr(4,2) == "LS") {
        cap(socket, "CAP * LS :\r\n");
        send(socket, welcomeMessage.c_str(), welcomeMessage.size(), 0); 
    }
}; 



void Server::pong(int socket, std::string message) {

    std::string buildMessage;
    
    buildMessage = "PONG ";
    buildMessage += message;
    buildMessage += "\r\n";
    
    send(socket, buildMessage.c_str(), buildMessage.size(), 0);
}


int Server::cap(int socket, std::string message) {

    std::string buildMessage;
    
    buildMessage = message;
    if ((send(socket, buildMessage.c_str(), buildMessage.size(), 0)) > 0) {
        return (0);
    }
    return (1);
};