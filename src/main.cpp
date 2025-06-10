/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:53:53 by nige42            #+#    #+#             */
/*   Updated: 2025/06/10 19:01:36 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"



int main(int argc, char *argv[]) {
    

    if (argc != 3) {
        std::cout << "Error" << std::endl;
        return(1);
    }
    signal(SIGINT, SigHandler::handleStop);
    //signal(SIGINT, signalHandler); // Catch Ctrl+C
    Server server(atoi(argv[1]), argv[2]);
    
    std::cout << "creatServer()" << std::endl;
    server.createServer(); 

    
    std::cout << "userLoopStart called" << std::endl;
    server.userLoopCheck();
   
    
    return 0;
}