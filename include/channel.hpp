/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 06:44:33 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/19 07:22:34 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <vector>
#include "../include/user.hpp"

class User;

class Channel {

    public:

        Channel(std::string name);
        ~Channel(void);

        std::string getChannelName(void);
        void setChannelName(std::string name);


    private:

        std::string channelName_;
        std::string channelPassword_;
        std::string channelTopic_;


        bool I_;
        bool T_;
        bool K_;
        bool O_;
        bool L_;

    
};