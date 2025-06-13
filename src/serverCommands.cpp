/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 08:36:07 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/13 10:08:15 by nrobinso         ###   ########.fr       */
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
}

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
}


void Server::serverWallopsCommand(size_t commandType, ssize_t bytesRead, std::string &buffer) {
    
    std::string message;  

     if (commandType == NOTICE) {
            message += "[SERVER]:";
            message += buffer.substr(8, bytesRead);
            message += "\n";
            if (message.size() > 20) {
                sendMessageAll(message);  // Send message to all clients
            }
     }
};


// Needed, makes it possible to send commands from terminal of the server.
void Server::makeServerStdinNonBlocking(void) {
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
};