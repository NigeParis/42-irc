/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 14:04:16 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/20 11:08:46 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parsing.hpp"


int Server::initClientsNames(int clientFd, std::string &inputClient, User &user) {

    static bool initNames = false;
    //User *user = findUserByFd(clientFd);
    std::string realName = extractClientData(inputClient, "USER ");
    realName = extractRealName(realName);
    std::string nickName = extractClientData(inputClient, "NICK ");
    std::string userPassWord = extractClientData(inputClient, "PASS ");
    std::string userName = extractClientData(inputClient, "USER ");
    userName = trimUserName(userName);


        //std::cout << BLUE << "[DEBUG] - userName: " << userName << " nickName: " << nickName << RESET << std::endl;
    
    if (initNames == false) {
        nickCommand(clientFd, nickName);        
        user.setNickName(nickName);
        user.setUserPassword(userPassWord); 
        user.setRealName(realName);
        user.setUserName(userName); 
        initNames = true; 
    }
    if ((userPassWord != this->password_) && (user.getValidPassword() == false)){
        user.setValidPassword(false);
        //std::vector<User*>::iterator it = std::find(users_.begin(), users_.end(), user);
        std::vector<User*>::iterator it = std::find(users_.begin(), users_.end(), &user);
        
        std::string userName = user.getNickName();
        putErrorMessage(clientFd, userName, "password incorrect", 464);
        Server::timeStamp(); 
        std::cout << BLUE << "[LOGOUT]    " << RESET << "<" << GREEN << std::setfill(' ') << std::setw(8)  << user.getNickName() << RESET << ">" << " Just left " << std::endl;
        putErrorMessage(clientFd, userName, "connection failed", 464);
        this->lastClientToWrite_ = 0;  
        close(clientFd);
        delete &user;
        if (it != users_.end()) {
            Server::timeStamp(); 
            //std::cout << YELLOW << "[CLIENTS]   " << RESET << "<" << std::setfill(' ') << std::setw(7) << YELLOW  << "active"  << RESET << "> " << users_.size() << std::endl;
            users_.erase(it);
        }            
        std::cout << YELLOW << "[CLIENTS]   " << RESET << "<" << std::setfill(' ') << std::setw(7) << YELLOW  << "active"  << RESET << "> " << users_.size() << std::endl;
        return (1) ;
    }
        
    user.setValidPassword(true);    
    return (0);
}

void Server::clientInputCommand(int clientFd, std::string &inputClient) {

    keyWordInput keyWordIn;

    
    if (inputClient.empty()) return;
    
    User *user = findUserByFd(clientFd);
    size_t start = inputClient.find_first_not_of(" \t");
    size_t end = inputClient.find_first_of(" \t");
    keyWordIn = getKeyWord(inputClient, start, end);
    std::string message = inputClient.substr(end - start + 1, inputClient.size()); 
    if (initClientsNames(clientFd, inputClient, *user)) return ;
    
    try {
        switch (keyWordIn.value) {
            case PING:
                pong(clientFd, message);
                break;

            case CAP: {
                cap(clientFd, inputClient, user);
                break;
            }
            case NICK: {

                nick(clientFd, inputClient, user);
                break;
            }
            case MODE:
                //std::cout << RED << "inside MODE" << RESET << std::endl;
                break;

            case QUIT:
                clientQuits(clientFd, *user);
                break;

            case WHOIS:
                //std::cout << BLUE << "[DEBUG] - WHOIS CMD RECIEVED" << RESET << std::endl;
                break;

            case ERROR:
                throw std::runtime_error("ERROR :No existing command\n");
        }
    }
    catch (const std::exception& e) {
        send(clientFd, e.what(), strlen(e.what()), 0);
    }


    
}; 

///////////////////////////////// Commands //////////////////////////////////////////////

int Server::sendCommand(int clientFd, std::string commandToBeSent) {

    if ((send(clientFd, commandToBeSent.c_str(), commandToBeSent.size(), 0)) > 0) {
        return (0);
    }
    return (1);
};


void Server::pong(int clientFd, std::string input) {

    std::string buildMessage;
    
    buildMessage = "PONG ";
    buildMessage += input;
    buildMessage += "\r\n";
    sendCommand(clientFd, buildMessage);
}


void Server::cap(int clientFd, std::string &inputClient, User *user) {
    
    std::string welcomeMessage = putWelcomeMessage(user);
    std::string nickName = extractClientData(inputClient, "NICK ");   
    std::string userName = extractClientData(inputClient, "USER ");
    bool nameDoubleFlag = false;
    userName = trimUserName(userName);

    if (inputClient.substr(4, 2) == "LS") {
        sendCommand(clientFd, "CAP * LS :\r\n");
        sendCommand(clientFd, welcomeMessage);
        for (size_t i = 0; i < users_.size(); ++i) {
            if ((users_[i]->getNickName() == nickName) && !nickName.empty()) {
                std::cout << BLUE << "DOUBLE FOUND !" << RESET << std::endl;
                nameDoubleFlag = true;
            }
        }
        if (!nameDoubleFlag) {
            nickCommand(clientFd, nickName);
        }
    }
}



void Server::nick(int clientFd, std::string &inputClient, User *user) {

    (void)user;
    std::string nickName = extractClientData(inputClient, "NICK ");
    bool nameDoubleFlag = false;

    for (size_t i = 0; i < users_.size(); ++i) {
        if ((users_[i]->getNickName() == nickName) && !nickName.empty()) {
            std::cout << BLUE << "DOUBLE FOUND !" << RESET << std::endl;
            putErrorMessage(clientFd, nickName, "Nickname is already in use", 433);
            nameDoubleFlag = true;
        }
    }
    if (!nameDoubleFlag) {
        nickCommand(clientFd, nickName);
        // user.setUserName(nickName);
    }
}

int Server::nickCommand(int clientFd, std::string input) {
    
    std::string message;
    User *user = findUserByFd(clientFd);
    
    if(checkLeadingHash(clientFd, input))
        return (1);
    if (checkForSpaces(clientFd, input))
        return (1);
    if (input == user->getNickName())
        return (0);     
    message = ":" + user->getNickName();
    message += " NICK " + input;
    message += "\r\n";
    Server::timeStamp(); 
    std::cout << RED << "[MESSAGE]   " << RESET << "<" << GREEN << std::setfill(' ') << std::setw(8) << user->getNickName() << RESET << "> ";
    std::cout << "Changed nickname to " << "<" << GREEN << input << RESET << ">" << std::endl;
    user->setNickName(input);
    sendCommand(clientFd, message);
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
            std::string errorMessage = "Error " + user->getNickName() + " " + input + " :Erroneus nickname";
            Server::timeStamp(); 
            std::cout << RED << "[MESSAGE]   " << RESET << "<" << GREEN << std::setfill(' ') << std::setw(8) << user->getNickName() << RESET << "> ";
            std::cout << errorMessage << std::endl;
            errorMessage += "\r\n";    //commande ended signal
            send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
            return (1); 
        }      
    }
    return (0);
};


void Server::putErrorMessage(int clientFd, std::string &nick, std::string errorMsg, int code) {

    User *user = findUserByFd(clientFd);
    
    // "<client> :No nickname given"
    if ((code == 431) || (code == 464) || (code == 433)) {
        std::string errorMessage = "Error " + user->getNickName() + " " + errorMsg;
        Server::timeStamp(); 
        std::cout << RED << "[MESSAGE]   " << RESET << "<" << GREEN << std::setfill(' ') << std::setw(8) << user->getNickName() << RESET << "> ";
        std::cout << errorMessage << std::endl;
        errorMessage += "\r\n";    //commande ended signal
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
    }
    
    // "<client> <nick> :Erroneus nickname"
    if (code == 432) {
        std::string errorMessage = "Error " + user->getNickName() + " " + nick + errorMsg;
        Server::timeStamp(); 
        std::cout << RED << "[MESSAGE]   " << RESET << "<" << GREEN << std::setfill(' ') << std::setw(8) << user->getNickName() << RESET << "> ";
        std::cout << errorMessage << std::endl;
        errorMessage += "\r\n";   //commande ended signal
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
    }    

    // RPL_ENDOFWHOIS (318) 
    //  "<client> <nick> :End of /WHOIS list"
    if (code == 318) {
        std::string targetNick = nick;  // the nick being looked up
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



std::string Server::putWelcomeMessage(User *user) {

    std::string welcomeMessage = "001 " 
                                + user->getNickName() 
                                + " :Welcome " 
                                + user->getNickName() 
                                + "\n" 
                                +  putClientBanner() 
                                + "\r\n";
    return (welcomeMessage);
};


keyWordInput getKeyWord(std::string &inputClient, size_t start, size_t end) {

    keyWordInput keyWord;

    std::string keyWordInput = inputClient.substr(start, end - start);    
    
    if (keyWordInput == "PING") {
        keyWord.value = PING;        
    }
    else if (keyWordInput == "CAP") {
        keyWord.value = CAP;        
    }
    else if (keyWordInput == "NICK") {
        keyWord.value = NICK;        
    }
    else if (keyWordInput == "MODE") {
        keyWord.value = MODE;        
    }
    else if (keyWordInput == "QUIT") {
        keyWord.value = QUIT;        
    }
    else if (keyWordInput == "WHOIS") {
        keyWord.value = WHOIS;        
    }
    else
        keyWord.value = ERROR;        
    
    return (keyWord);
}
