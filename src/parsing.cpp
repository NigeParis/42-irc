/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 14:04:16 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/15 15:57:07 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parsing.hpp"

void Server::printInputCommand(int socket, std::string &inputClient) {

    size_t start = inputClient.find_first_not_of(" \t");
    if (start == std::string::npos) return ;
    size_t end = inputClient.find_first_of(" \t");
    std::string result = inputClient.substr(start, end - start);    
     std::cout << "Size: " << inputClient.size() << std::endl;
    std::string message = inputClient.substr(end - start + 1, inputClient.size());    

    
    //std::cout << "testing function printCMD: " << result << std::endl;
    
    if (result == "PING") {
        pong(socket, message);        
    }    

    
    
}; 
