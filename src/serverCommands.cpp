/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 08:36:07 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/16 08:34:52 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/serverCommands.hpp"

void Server::runServerCommands(void) {
    
    std::string message;  
    std::string buffer(BUFFER, '\0');
    ssize_t bytesRead = read(STDIN_FILENO, &buffer[0], BUFFER);
    
    if (bytesRead > 2) {
        buffer[bytesRead - 1] = '\0';  // Remove newline character
        int commandType = ServerCommandStartsWith(buffer);
        serverWallopsCommand(commandType, bytesRead, buffer);
        serverExitCommand(commandType, bytesRead, buffer);
    }  
};

size_t Server::ServerCommandStartsWith(const std::string &str) {
    
    if (str.find("/wallops ") == 0)
        return (NOTICE);
    if (str.find("/squit ") == 0)
        return (EXIT);

    std::string command = str.substr(0,6);
    if (command == "/squit")
        return (EXIT);
    return (0);
};

void Server::serverExitCommand(size_t commandType, ssize_t bytesRead, std::string &buffer) {

    std::string message;  
    
    if (commandType == EXIT) {
        std::cout << bytesRead << std::endl;
        message += "[SERVER]:";
        message += buffer.substr(6, bytesRead);
        message += "\n";
        if (message.size() > 20) {
            sendMessageAll(message);  // Send message to all clients
        }
        SigHandler::sigloop = false;
    }
};


void Server::serverWallopsCommand(size_t commandType, ssize_t bytesRead, std::string &buffer) {
    
    std::string message;  

     if (commandType == NOTICE) {
            message += buffer.substr(9, bytesRead);
            message += "\r\n";
            sendMessageAll(message);  // Send message to all clients
     }
};


// Needed, makes it possible to send commands from terminal of the server.
void Server::makeServerStdinNonBlocking(void) {
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
};



void Server::pong(int socket, std::string message) {

    std::string buildMessage;
    
    buildMessage = "PONG ";
    buildMessage += message;
    buildMessage += "\r\n";
    
    send(socket, buildMessage.c_str(), buildMessage.size(), 0);
    //std::cout << "Pong message: " << "'" << buildMessage << "'" << std::endl;
    
}


int Server::cap(int socket, std::string message) {

    std::string buildMessage;
    
    buildMessage = message;
    //buildMessage += "\r\n";
    
    if ((send(socket, buildMessage.c_str(), buildMessage.size(), 0)) > 0) {

    
        return (0);
    }
    
    return (1);
};