#include "../include/SigHandler.hpp"
#include <iostream>

bool SigHandler::sigloop = true;

SigHandler::SigHandler(void){};
SigHandler::~SigHandler(void){};

void SigHandler::handleStop(int signal) {
    std::cout << "kill triggered" << std::endl;
    if (signal == SIGINT) {
        std::cout << "killing process" << std::endl;
        SigHandler::sigloop = false;
        //std::cout << "new sig value = " << SigHandler::sigloop << std::endl;
    }
}