/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 14:04:16 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/21 10:51:45 by nrobinso         ###   ########.fr       */

/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

int Server::initClientsNames(int clientFd, std::string &inputClient, User &user) {

    static bool initNames = false;
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
    std::string testmessage;

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
                std::cout << BLUE << "[DEBUG] - case: MODE" << RESET << std::endl;
                break;

            case QUIT:
                clientQuits(clientFd, *user);
                break;

            case JOIN:
                std::cout << BLUE << "[DEBUG] - case: JOIN" << RESET << std::endl;
                join(clientFd, inputClient, user);
                break;

            case WHOIS:
                std::cout << BLUE << "[DEBUG] - case: WHOIS" << RESET << std::endl;
                break;

            case PRIVMSG:
                std::cout << BLUE << "[DEBUG] - case: PRIVMSG" << RESET << std::endl;
                break;
                
            case USER:
                std::cout << BLUE << "[DEBUG] - case: USER" << RESET << std::endl;
                break;

            case TOPIC:
                std::cout << BLUE << "[DEBUG] - case: TOPIC" << RESET << std::endl;
                topic(clientFd, inputClient, user);
                break;

            case PASS:
                std::cout << BLUE << "[DEBUG] - case: PASS" << RESET << std::endl;
                break;

            case INVITE:
                std::cout << BLUE << "[DEBUG] - case: INVITE" << RESET << std::endl;
                break;
            case PART:
                std::cout << BLUE << "[DEBUG] - case: PART" << RESET << std::endl;
                part(clientFd, inputClient, user);
                break;  
            case KICK:
                std::cout << BLUE << "[DEBUG] - case: KICK" << RESET << std::endl;
                break;

            case LIST:
                std::cout << BLUE << "[DEBUG] - case: LIST" << RESET << std::endl;
                for (std::vector<Channel*>::iterator it = channels_.begin(); it < channels_.end(); it++) {
                    std::cout << BLUE << "[DEBUG] - channel[i] output: " << (*it)->getChannelName() << RESET << std::endl;
                }
                break;

            case WHO:
                std::cout << BLUE << "[DEBUG] - case: WHO" << RESET << std::endl;
                sendCommand(clientFd, ":localhost 352 nrobinso test nrobinso localhost localhost nrobinso H@ :0 Nigel Robinson\r\n");
                break;

            case ERROR:
                throw std::runtime_error("ERROR :No existing command\n");
        }
    }
    catch (const std::exception& e) {
        sendCommand(clientFd, e.what());
    }
}; 



int Server::sendCommand(int clientFd, std::string commandToBeSent) {

    if ((send(clientFd, commandToBeSent.c_str(), commandToBeSent.size(), 0)) > 0) {
        return (0);
    }
    return (1);
};

void Server::part(int clientFd, std::string &inputClient, User *user) {

  std::string channel = extractClientData(inputClient, "PART ");


  std::string message = ":" + user->getNickName() + "!nrobinso@localhost PART " + channel +"\r\n";
                std::cout << message << std::endl;
                send(clientFd, message.c_str(), message.size(), 0);       
}


std::vector<Channel*>::iterator Server::findTopicByChannelName(std::string name) {
    for (std::vector<Channel*>::iterator it = channels_.begin(); it != channels_.end(); it++) {
        if ((*it)->getChannelName() == "#" + name) {
            return it;
        }
    }
    return channels_.begin();
}


void Server::topic(int clientFd, std::string &inputClient, User *user) {

    (void)clientFd;
    (void)inputClient;
    (void)user;
    std::string serverName = "localhost";
    std::string topic = extractClientData(inputClient, "TOPIC ");
    std::string channel = extractChannel(topic);
    User *nick = findUserByFd(clientFd);
    topic = extractTopic(topic);


    
    std::string message = ":" + serverName + " 332 " + nick->getNickName() + " #" + channel + " :" + topic + "\r\n";
    std::cout << BLUE << "[DEBUG] - " << message << RESET << std::endl;
    sendCommand(clientFd, message);
    
    std::cout << GREEN << "[DEBUG] - topic(): " << (*findTopicByChannelName(channel))->getChannelTopic() << RESET << std::endl;;


    
};

int Server::findByChannelName(std::string name) {
    for (size_t i = 0; i < channels_.size(); i++) {
        if (channels_[i]->getChannelName() == name)
            return i;
    }
    return -1; // Not found
}





void Server::join(int clientFd, std::string &inputClient, User *user) {

    std::string channel = extractClientData(inputClient, "JOIN ");
    std::string nick = user->getNickName();
    std::string serverName = "localhost";
    std::string message;

    Channel *newChannel = new Channel(channel);

   
    // 332 RPL_TOPIC (prefix = server)

    
    message = ":" + serverName + " 332 " + nick + " " + channel + " :" + newChannel->getChannelTopic() + "\r\n";
    std::cout << BLUE << "[DEBUG] - " << message << RESET << std::endl;
    sendCommand(clientFd, message);

    // 333 RPL_TOPICWHOTIME (prefix = server)
    // message = ":" + serverName + " 333 " + nick + " " + channel + " admin!nrobinso@localhost 1719055420\r\n";
    // sendCommand(clientFd, message);

    // // 353 RPL_NAMREPLY
    // message = ":" + serverName + " 353 " + nick + " = " + channel + " :@admin +user1 " + nick + "\r\n";
    // sendCommand(clientFd, message);

    // 366 RPL_ENDOFNAMES
    // message = ":" + serverName + " 366 " + nick + " " + channel + " :End of /NAMES list.\r\n";
    // sendCommand(clientFd, message);

    // message = ":" + serverName + " 324 " + nick + " " + channel + " +nt\r\n";
    // sendCommand(clientFd, message);

    
    std::cout << BLUE << "[DEBUG] - channelName: " << newChannel->getChannelName() << RESET << std::endl;
    std::cout << BLUE << "[DEBUG] - channelPassword: " << newChannel->getChannelPassword() << RESET << std::endl;
    std::cout << BLUE << "[DEBUG] - channelTopic: " << newChannel->getChannelTopic() << RESET << std::endl;
    
    this->channels_.push_back(newChannel);
    std::cout << YELLOW << "[DEBUG] - channelSize: " << this->channels_.size() << RESET << std::endl;
    std::cout << YELLOW << "[DEBUG] - channelSize: " << this->channels_[findByChannelName(channel)]->getChannelName() << RESET << std::endl;

}



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
                //std::cout << BLUE << "DOUBLE FOUND !" << RESET << std::endl;
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

    std::cout << YELLOW << "[DEBUG] - " << input << RESET << std::endl;

    if (user->getNickName().empty() || input.empty()) {
        putErrorMessage(clientFd, input, " :No nickname given", 431);        
        return (1);
    }
    if(checkLeadingHash(clientFd, input)) {
        std::cout << YELLOW << "[DEBUG] - checkLeadHash()" << input << RESET << std::endl;
        return (1);
    }
    if (checkForSpaces(clientFd, input)) {
        std::cout << YELLOW << "[DEBUG] - checkForSpaces()" << input << RESET << std::endl;
        return (1);
    }
    if (input == user->getNickName()) {
        std::cout << YELLOW << "[DEBUG] - input == nickName" << input << RESET << std::endl;
        return (0);
    }
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
    else if (keyWordInput == "JOIN") {
        keyWord.value = JOIN;        
    }
    else if (keyWordInput == "PRIVMSG") {
        keyWord.value = PRIVMSG;        
    }
    else if (keyWordInput == "USER") {
        keyWord.value = USER;        
    }
    else if (keyWordInput == "KICK") {
        keyWord.value = KICK;        
    }
    else if (keyWordInput == "TOPIC") {
        keyWord.value = TOPIC;
    }        
    else if (keyWordInput == "PASS") {
        keyWord.value = PASS;        
    }
    else if (keyWordInput == "PART") {
        keyWord.value = PART;        
    }
    else if (keyWordInput == "WHO") {
        keyWord.value = WHO;        
    }
    else if (keyWordInput == "LIST") {
        keyWord.value = LIST;        
    }
    else if (keyWordInput == "INVITE") {
        keyWord.value = INVITE;        
    }
    else
    keyWord.value = ERROR;        
    
    return (keyWord);
}

/////////////////////////// output Display Functions ////////////////////////////////////////////////


void Server::putErrorMessage(int clientFd, std::string &nick, std::string errorMsg, int code) {

    User *user = findUserByFd(clientFd);
    
    // "<client> :No nickname given"
    if ((code == 431) || (code == 464) || (code == 433)) {
        std::string errorMessage = "Error " + user->getNickName() + " " + errorMsg;
        Server::timeStamp(); 
        std::cout << RED << "[MESSAGE]   " << RESET << "<" << GREEN << std::setfill(' ') << std::setw(8) << user->getNickName() << RESET << "> ";
        std::cout << errorMessage << std::endl;
        errorMessage += "\r\n";    //commande ended signal
        sendCommand(clientFd, errorMessage);
    }
    
    // "<client> <nick> :Erroneus nickname"
    if (code == 432) {
        std::string errorMessage = "Error " + user->getNickName() + " " + nick + errorMsg;
        Server::timeStamp(); 
        std::cout << RED << "[MESSAGE]   " << RESET << "<" << GREEN << std::setfill(' ') << std::setw(8) << user->getNickName() << RESET << "> ";
        std::cout << errorMessage << std::endl;
        errorMessage += "\r\n";   //commande ended signal
        sendCommand(clientFd, errorMessage);
    }    

    // RPL_ENDOFWHOIS (318) 
    //  "<client> <nick> :End of /WHOIS list"
    if (code == 318) {
        std::string targetNick = nick;  // the nick being looked up
        std::string requesterNick = user->getNickName();  // the one who issued /WHOIS
        std::string messageText = ":localhost 318 " + requesterNick + " " + targetNick + " :End of /WHOIS list";        
        messageText += "\r\n";
        sendCommand(clientFd, messageText);
    }
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
