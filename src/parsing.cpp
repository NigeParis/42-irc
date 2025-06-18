/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 14:04:16 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/18 18:48:24 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parsing.hpp"


int Server::initClientsNames(int clientFd, std::string &inputClient) {

    static bool initNames = false;

    User *user = findUserByFd(clientFd);
    std::string nickName = extractClientData(inputClient, "NICK ");
    std::string userPassWord = extractClientData(inputClient, "PASS ");
    std::string userName = extractClientData(inputClient, "USER ");
    if (initNames == false) {
        user->setNickName(nickName);
        user->setUserPassword(userPassWord); 
        std::string userName = extractClientData(inputClient, "USER ");
        userName = trimUserName(userName);
        user->setUserName(userName); 
        std::string realName = extractClientData(inputClient, "USER ");
        realName = extractRealName(realName);
        user->setRealName(realName);          
    }
    if ((userPassWord != this->password_) && (user->getValidPassword() == false)){
        user->setValidPassword(false);
        std::vector<User*>::iterator it = std::find(users_.begin(), users_.end(), user);
        std::string userName = user->getNickName();
        putErrorMessage(clientFd, userName, "password incorrect", 464);
        Server::timeStamp(); 
        std::cout << BLUE << "[LOGOUT]    " << RESET << "<" << GREEN << user->getNickName() << RESET << ">" << " exiting from the network" << std::endl;
        putErrorMessage(clientFd, userName, "connection failed", 464);
        this->lastClientToWrite_ = 0;  
        close(clientFd);
        delete user;
        if (it != users_.end()) {
            Server::timeStamp(); 
            std::cout << YELLOW << "[CLIENTS]   " << RESET << "<" << GREEN << "active" << RESET << "> " << users_.size() << std::endl;
            users_.erase(it);
        }            
        return (1) ;
    }
    user->setValidPassword(true);
    initNames = true;

    return (0);
}


// info 
// PASS passWord
// NICK nickName
// USER username hostname servername :realname

void Server::clientInputCommand(int clientFd, std::string &inputClient) {

    
    if (inputClient.empty()) return;

    
    User *user = findUserByFd(clientFd);
    size_t start = inputClient.find_first_not_of(" \t");
    size_t end = inputClient.find_first_of(" \t");
    std::string keyWordInput = inputClient.substr(start, end - start);    
    std::string message = inputClient.substr(end - start + 1, inputClient.size()); 
    if (initClientsNames(clientFd, inputClient))
        return ;
    std::string welcomeMessage = "001 " + user->getNickName() + " :Welcome " + user->getNickName() + "\n" +  putClientBanner() + "\r\n";
   

    try
    {
        if (keyWordInput == "PING") {
            pong(clientFd, message);        
        }    
        else if (keyWordInput == "CAP") {
            if ((keyWordInput == "CAP") && inputClient.substr(4,2) == "LS") {
                cap(clientFd, "CAP * LS :\r\n");
                send(clientFd, welcomeMessage.c_str(), welcomeMessage.size(), 0); 
            }
        }        
        else if (keyWordInput == "NICK") {
            nick(clientFd, user->getNickName());        
        }
        else if (keyWordInput == "MODE") {
            
            std::cout << RED "inside MODE" << RESET << std::endl;
        }
        else if (keyWordInput == "QUIT") {
                    clientQuits(clientFd, *user);
        }
        else {
            std::cout << BLUE << keyWordInput << RESET << std::endl;
            throw std::runtime_error("ERROR :No existing command\n");
        }
    }
    catch(const std::exception& e)
    {
        send(clientFd, e.what(), strlen(e.what()), 0);
    }


    
    
}; 

///////////////////////////////// Commands //////////////////////////////////////////////


void Server::pong(int clientFd, std::string input) {

    std::string buildMessage;
    
    buildMessage = "PONG ";
    buildMessage += input;
    buildMessage += "\r\n";
    send(clientFd, buildMessage.c_str(), buildMessage.size(), 0);
}


int Server::cap(int clientFd, std::string input) {

    std::string buildMessage;
    
    buildMessage = input;
    if ((send(clientFd, buildMessage.c_str(), buildMessage.size(), 0)) > 0) {
        return (0);
    }
    return (1);
};



int Server::nick(int clientFd, std::string input) {
    
    std::string message;
    User *user = findUserByFd(clientFd);
    
    if(checkLeadingHash(clientFd, input))
        return (1);
    if (checkForSpaces(clientFd, input))
        return (1);
    
    message = ":" + user->getNickName();
    message += " NICK " + input;
    message += "\r\n";
    Server::timeStamp(); 
    std::cout << RED << "[MESSAGE]   " << RESET << "<" << GREEN << user->getNickName() << RESET << "> ";
    std::cout << "Changed nickname to " << "<" << GREEN << input << RESET << ">" << std::endl;
    user->setNickName(input);
    send(clientFd, message.c_str(), message.size(), 0);
    return (0);
};




//////////////////////////////////////// Parsing tools ////////////////////////////////////////


int Server::checkForSpaces(int clientFd, std::string &input) {

    User *user = findUserByFd(clientFd);

    if (input.empty()) {
        return (1);
    }
    for (std::string::iterator it = input.begin(); it < input.end(); it++) {
        if (*it == ' ') {
            std::string errorMessage = "Error " +user->getNickName() + " " + input + " :Erroneus nickname";
            Server::timeStamp(); 
            std::cout << RED << "[MESSAGE]   " << RESET << "<" << GREEN << user->getNickName() << RESET << "> ";
            std::cout << errorMessage << std::endl;
            errorMessage += "\r\n";    //commande ended signal
            send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
            return (1); 
        }      
    }
    return (0);
};


void Server::putErrorMessage(int clientFd, std::string &input, std::string errorMsg, int code) {

    User *user = findUserByFd(clientFd);
    
    // "<client> :No nickname given"
    if ((code == 431) || (code == 464)) {
        std::string errorMessage = "Error " + user->getNickName() + " " + errorMsg;
        Server::timeStamp(); 
        std::cout << RED << "[MESSAGE]   " << RESET << "<" << GREEN << user->getNickName() << RESET << "> ";
        std::cout << errorMessage << std::endl;
        errorMessage += "\r\n";    //commande ended signal
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
    }
    
    // "<client> <nick> :Erroneus nickname"
    if (code == 432) {
        std::string errorMessage = "Error " + user->getNickName() + " " + input + errorMsg;
        Server::timeStamp(); 
        std::cout << RED << "[MESSAGE]   " << RESET << "<" << GREEN << user->getNickName() << RESET << "> ";
        std::cout << errorMessage << std::endl;
        errorMessage += "\r\n";   //commande ended signal
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
    }    

    // RPL_ENDOFWHOIS (318) 
    //  "<client> <nick> :End of /WHOIS list"
    if (code == 318) {
        std::string targetNick = input;  // the nick being looked up
        std::string requesterNick = user->getNickName();  // the one who issued /WHOIS
        std::string messageText = ":localhost 318 " + requesterNick + " " + targetNick + " :End of /WHOIS list";        
        messageText += "\r\n";
        send(clientFd, messageText.c_str(), messageText.size(), 0);
    }
};




// Checks for leading #nick - interdiction error (432)
//ERR_ERRONEUSNICKNAME (432)

int Server::checkLeadingHash(int clientFd, std::string &input) {

    if (input.empty()) {
        putErrorMessage(clientFd, input, " :No nickname given", 431);        
        return (1);
    }
    if ((*input.begin() == '#') || (*input.begin() == ':')) {
        putErrorMessage(clientFd, input, " :Erroneus nickname", 432);
        return(1);
    }
    return (0);
};


std::string Server::trimUserName(std::string &userName) {

    std::string trimmedString;

    size_t pos = 0;
    size_t end = userName.find(" ");
    trimmedString = userName.substr(pos, end);
    return(trimmedString);   
}


std::string Server::extractClientData(std::string &input, std::string strFind) {

    std::string string;
    
    if (input.empty())
        return ("");
    std::size_t pos = input.find(strFind);
    if (pos == input.npos)
        return ("");
    std::size_t pStart = input.find(" ", pos) + 1;
    std::size_t pEnd = input.find('\r', pStart);
    string = input.substr(pStart, pEnd - pStart);
    return (string);
}


std::string Server::extractRealName(std::string &realName) {

    std::string newString;
    int pos;
    int end = realName.size();

    if (realName.empty())
        return ("");
    pos = realName.find(":") + 1;
    newString = realName.substr(pos, end);
    return (newString);  
};

