/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 14:02:07 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/20 11:04:51 by nrobinso         ###   ########.fr       */
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
    ERROR
};

struct keyWordInput {

    keyWord value;  
};

keyWordInput getKeyWord(std::string &inputClient, size_t start, size_t end);

