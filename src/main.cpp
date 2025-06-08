/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nige42 <nige42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:53:53 by nige42            #+#    #+#             */
/*   Updated: 2025/06/08 17:59:46 by nige42           ###   ########.fr       */
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


    server->makeUser();
    server->makeUser();
    

    sleep(10);    
    while (1) {
        sleep(5);
        std::cout << "reading message" << std::endl;
        server->readMessage(*server->users_[0]);
        server->readMessage(*server->users_[1]);
        server->sendMessage(*server->users_[0], "Coucou zero server here \n");
        server->sendMessage(*server->users_[1], "Coucou Un server here \n");
    }

    
    delete server;
    return 0;
}