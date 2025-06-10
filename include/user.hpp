/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 15:01:27 by nige42            #+#    #+#             */
/*   Updated: 2025/06/10 09:14:43 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <poll.h>

class User {

    public:

        User (int fd, short events, short revents);
        ~User(void);

        void setUserFd(int fd);
        int getUserFd(void);

        struct pollfd user_pollfd;
        
    private:

        int socketFd_;
        
    
    
};