/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 14:02:07 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/22 09:39:54 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include "server.hpp"
#include "parsingTools.hpp"

/// @brief enum used in the function getKeyWord()
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

/// @brief stores the value of keyword.value using an enum keyword
/// @brief used in the function getKeyWord();
struct keyWordInput {
    keyWord value;  
};

/// @brief get the key word inputed from the terminal client ex: "NICK"
/// @brief needs keyWordInput structure
/// @param inputClient line inputed from clients terminal
/// @param start starting point of the keyword
/// @param end ending point of the keyword
/// @return the keyword as a structure keyword
keyWordInput getKeyWord(std::string &inputClient, size_t start, size_t end);
std::string extractChannel(std::string &topic);
std::string extractTopic(std::string &topic);
