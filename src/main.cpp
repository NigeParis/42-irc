/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:53:53 by nige42            #+#    #+#             */
/*   Updated: 2025/06/10 09:51:42 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

int main(int argc, char *argv[]) {

    Server *server = 0;

    if (argc != 3) {
        std::cout << "Error" << std::endl;
        return(1);
    }
    
    server = new Server(atoi(argv[1]), argv[2]);
    
    server->createServer(); 

    std::cout << "makeUser()" << std::endl;

    server->makeUser();
    server->makeUser();
    

    
    std::cout << "userLoopStart called" << std::endl;
    server->userLoopCheck();
   
    
    delete server;
    return 0;
}