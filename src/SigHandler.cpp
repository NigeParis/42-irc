/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SigHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:01:48 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/11 17:42:43 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/SigHandler.hpp"
#include <iostream>

bool SigHandler::sigloop = true;

SigHandler::SigHandler(void){};
SigHandler::~SigHandler(void){};

void SigHandler::handleStop(int signal) {
    std::cout << "\nkill triggered" << std::endl;
    if (signal == SIGINT) {
        std::cout << "killing process" << std::endl;
        SigHandler::sigloop = false;
    }
}
