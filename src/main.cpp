/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nige42 <nige42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:53:53 by nige42            #+#    #+#             */
/*   Updated: 2025/06/06 15:35:21 by nige42           ###   ########.fr       */
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

    while (1) {
        sleep(5);
        server->readMessage(server->users_[0] );
    }

    
    delete server;
    return 0;
}