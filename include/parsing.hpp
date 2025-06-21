/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 14:02:07 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/21 16:06:23 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include "server.hpp"


enum keyWord {

    PING,
    CAP,
    NICK,
    MODE,
    QUIT,
    WHOIS,
    JOIN,
    PRIVMSG,
    USER,
    PASS,
    TOPIC,
    INVITE,
    KICK,
    ERROR,
    PART,
    WHO,
    LIST
};

struct keyWordInput {
    keyWord value;  
};

keyWordInput getKeyWord(std::string &inputClient, size_t start, size_t end);
std::string extractChannel(std::string &topic);
std::string extractTopic(std::string &topic);
