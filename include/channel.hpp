/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 06:44:33 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/21 09:09:57 by nrobinso         ###   ########.fr       */
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

        std::string getChannelPassword(void);
        void setChannelPassword(std::string name);

        std::string getChannelTopic(void);
        void setChannelTopic(std::string name);


        std::vector<User*> channelmembre_;
        std::vector<User*> channelInvited_;
        std::vector<User*> channelBoss_;

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