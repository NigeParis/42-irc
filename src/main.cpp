/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nige42 <nige42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:53:53 by nige42            #+#    #+#             */
/*   Updated: 2025/06/05 10:34:48 by nige42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cout << "Error" << std::endl;
        return(1);
    }
    
    Server server(atoi(argv[1]), argv[2]);
    
    server.createServer(); 
    return 0;
}