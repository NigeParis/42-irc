/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 06:44:33 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/22 09:30:33 by nrobinso         ###   ########.fr       */
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

        /// @brief stores the users of the channel that are membres, invited and adminstrators
        std::vector<User*> channelmembre_;
        std::vector<User*> channelInvited_;
        std::vector<User*> channelBoss_;

    private:

        /// @brief used to strore the name of the channel
        std::string channelName_;
        /// @brief used to store a password needed to acess a channel
        std::string channelPassword_;
        /// @brief used to store the topic title of the channel
        std::string channelTopic_;

        /// @brief used with cmd MODE
        /// @brief boolenas to turn onm or off options of the channel
        /// @brief exe: I_ = true - invite only channel
        bool I_;
        bool T_;
        bool K_;
        bool O_;
        bool L_;

    
};