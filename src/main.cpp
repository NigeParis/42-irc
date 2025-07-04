/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:53:53 by nige42            #+#    #+#             */
/*   Updated: 2025/06/21 11:04:02 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <iostream>

int main(int argc, char *argv[]) {

  if (argc != 3) {
    std::cout << "Error: ./ircserv <port> <password>" << std::endl;
    return (1);
  }
  signal(SIGINT, SigHandler::handleStop);
  if (atoi(argv[1]) < 1024 || atoi(argv[1]) > 65535) {
    std::cerr << "Error: Port number must be between 1024 and 65535." << std::endl;
    return (1);
  }
  Server server(atoi(argv[1]), argv[2]);
  server.createServer();
  server.userLoopCheck();

  return 0;
}
