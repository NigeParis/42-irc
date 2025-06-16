/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 14:04:16 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/16 10:06:37 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parsing.hpp"

void Server::clientInputCommand(int socket, std::string &inputClient) {

    if (inputClient.empty()) return;

    size_t start = inputClient.find_first_not_of(" \t");
    size_t end = inputClient.find_first_of(" \t");
    std::string result = inputClient.substr(start, end - start);    
    std::string message = inputClient.substr(end - start + 1, inputClient.size());    

    std::string welcomeMessage = "001 :Welcome \n" +  putClientBanner() + "\r\n";




    
    
    if (result == "PING") {
        pong(socket, message);        
    }    
    if ((result == "CAP") && inputClient.substr(4,2) == "LS") {
        cap(socket, "CAP * LS :\r\n");
        send(socket, welcomeMessage.c_str(), welcomeMessage.size(), 0);
       
        
    }
}; 
